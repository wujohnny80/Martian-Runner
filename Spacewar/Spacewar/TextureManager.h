#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_
#define WIN32_LEAN_AND_MEAN

#include "Constants.h"
#include "Graphics.h"

class TextureManager
{
public:
	TextureManager();
	virtual ~TextureManager();

	virtual bool Initialize(Graphics *g, const char *file);
	virtual void OnLostDevice();
	virtual void OnResetDevice();

	LP_TEXTURE GetTexture() const { return texture; }
	UINT GetWidth() const { return width; }
	UINT GetHeight() const { return height; }

private:
	UINT width;				// width of texture in pixels
	UINT height;			// height of texture in pixels
	LP_TEXTURE texture;		// pointer to texture
	const char *file;		// name of file
	Graphics *graphics;		// save pointer to graphics
	bool initialized;		// true when successfully initialized
	HRESULT hr;				// standard return type
};
#endif // _TEXTURE_MANAGER_H_