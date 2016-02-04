#include "TextureManager.h"

TextureManager::TextureManager()
	: texture (NULL)
	, width (0)
	, height (0)
	, file (NULL)
	, graphics(NULL)
	, initialized (false)
{
}

//----------------------------------------------------------------------------------------------------

TextureManager::~TextureManager()
{
	SAFE_RELEASE(texture);
}

//----------------------------------------------------------------------------------------------------

bool TextureManager::Initialize(Graphics *g, const char *f)
{
	try
	{
		graphics = g;
		file = f;

		hr = graphics->LoadTexture(file, TRANSCOLOR, width, height, texture);
		if (FAILED(hr))
		{
			SAFE_RELEASE(texture);
			return false;
		}
	}
	catch(...) { return false; }
	initialized = true;
	return true;
}

//----------------------------------------------------------------------------------------------------

void TextureManager::OnLostDevice()
{
	if (!initialized)
		return;
	SAFE_RELEASE(texture);
}

//----------------------------------------------------------------------------------------------------

void TextureManager::OnResetDevice()
{
	if (!initialized)
		return;
	graphics->LoadTexture(file, TRANSCOLOR, width, height, texture);
}