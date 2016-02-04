#include "Graphics.h"

Graphics::Graphics()
	: direct3D (NULL)
	, device3D (NULL)
	, sprite (NULL)
	, fullscreen (false)
	, width (GAME_WIDTH)
	, height (GAME_HEIGHT)
{
	backColor = GraphicsNS::BACK_COLOR; // dark blue
}

//----------------------------------------------------------------------------------------------------

Graphics::~Graphics()
{
	ReleaseAll();
}

//----------------------------------------------------------------------------------------------------

void Graphics::Initialize(HWND hw, int w, int h, bool full)
{
	hwnd = hw;
	width = w;
	height = h;
	fullscreen = full;

	// Initialize Direct3D
	direct3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(direct3D == NULL)
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error Initializing Direct3D"));

	InitD3DPP(); // initialize D3D Presentation Parameters
	if(fullscreen)      // if full-screen mode
	{
		if(IsAdapterCompatible())   // is the adapter compatible
			// set the refresh rate with a compatible one
			d3DPP.FullScreen_RefreshRateInHz = pMode.RefreshRate;
		else
			throw(GameError(GameErrorNS::FATAL_ERROR, 
			"The graphics device does not support the specified resolution and/or format."));
	}

	// Determine if graphics card supports hardware texturing
	// and lighting and vertex shaders
	D3DCAPS9 caps;
	DWORD behavior;
	result = direct3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, & caps);

	// If device doesn't support HW T&L or doesn't support 1.1 vertex
	// shaders in hardware, then switch to software vertex processing
	if((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || caps.VertexShaderVersion < D3DVS_VERSION(1,1))
	{
		// Use software only processing
		behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else
	{
		behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}

	// Create Direct3D device
	result = direct3D->CreateDevice(D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									hwnd,
									behavior,
									&d3DPP,
									&device3D);

	if(FAILED(result))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));

	// Create Direct3D sprite
	result = D3DXCreateSprite(device3D, &sprite);
	if (FAILED(result))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error creating Direct3D sprite"));

	// Configure for alpha blend of primitives
    device3D->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    device3D->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device3D->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//----------------------------------------------------------------------------------------------------

HRESULT Graphics::LoadTexture(const char *filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture)
{
	// Load the texture into default D3D memory (normal texture use)
	// For internal engine use only. Use the TextureManager class to load game textures.
	// Pre: filename = name of texture file.
	//		transcolor = transparent color
	// Post: width and height = size of texture
	//		 texture points to texture

	// The struct for reading file info
	D3DXIMAGE_INFO info;
	result = E_FAIL;

	try
	{
		if(filename == NULL)
		{
			texture = NULL;
			return D3DERR_INVALIDCALL;
		}

		// Get width and height from file
		result = D3DXGetImageInfoFromFile(filename, &info);
		if (result != D3D_OK)
			return result;
		width = info.Width;
		height = info.Height;

		// Create the new texture by loading from file
		result = D3DXCreateTextureFromFileEx( device3D,				//3D device
											  filename,				//image filename
											  info.Width,			//texture width
											  info.Height,			//texture height
											  1,					//mip-map levels (1 for no chain)
											  0,					//usage
											  D3DFMT_UNKNOWN,		//surface format (default)
											  D3DPOOL_DEFAULT,		//memory class for the texture
											  D3DX_DEFAULT,			//image filter
											  D3DX_DEFAULT,			//mip filter
											  transcolor,			//color key for transparency
											  &info,				//bitmap file info (from loaded file)
											  NULL,					//color palette
											  &texture );			//destination texture
	}

	catch(...)
	{
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error in Graphics::LoadTexture"));
	}
	return result;
}

//----------------------------------------------------------------------------------------------------

HRESULT Graphics::LoadTextureSystemMem(const char *filename, COLOR_ARGB transcolor, 
                                    UINT &width, UINT &height, LP_TEXTURE &texture)
{
    // The struct for reading bitmap file info
    D3DXIMAGE_INFO info;
    result = E_FAIL;        // Standard Windows return value

    try{
        if(filename == NULL)
        {
            texture = NULL;
            return D3DERR_INVALIDCALL;
        }
 
        // Get width and height from bitmap file
        result = D3DXGetImageInfoFromFile(filename, &info);
        if (result != D3D_OK)
            return result;
        width = info.Width;
        height = info.Height;

        // Create the new texture by loading a bitmap image file
        result = D3DXCreateTextureFromFileEx( 
            device3D,           //3D device
            filename,           //bitmap filename
            info.Width,         //bitmap image width
            info.Height,        //bitmap image height
            1,                  //mip-map levels (1 for no chain)
            0,                  //usage
            D3DFMT_UNKNOWN,     //surface format (default)
            D3DPOOL_SYSTEMMEM,  //systemmem is lockable
            D3DX_DEFAULT,       //image filter
            D3DX_DEFAULT,       //mip filter
            transcolor,         //color key for transparency
            &info,              //bitmap file info (from loaded file)
            NULL,               //color palette
            &texture );         //destination texture

    } catch(...)
    {
        throw(GameError(GameErrorNS::FATAL_ERROR, "Error in Graphics::loadTexture"));
    }
    return result;
}

//----------------------------------------------------------------------------------------------------

HRESULT Graphics::CreateVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER &vertexBuffer)
{
    // Standard Windows return value
    HRESULT result = E_FAIL;

    // Create a vertex buffer
    result = device3D->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, D3DFVF_VERTEX,
                                            D3DPOOL_DEFAULT, &vertexBuffer, NULL);
    if(FAILED(result))
        return result;

    void *ptr;
    // must lock buffer before data can be transferred in
    result = vertexBuffer->Lock(0, size, (void**)&ptr, 0);
    if(FAILED(result))
        return result;
    memcpy(ptr, verts, size);   // copy vertex data into buffer
    vertexBuffer->Unlock();     // unlock buffer

    return result;
}

//----------------------------------------------------------------------------------------------------

bool Graphics::DrawQuad(LP_VERTEXBUFFER vertexBuffer)
{
    HRESULT result = E_FAIL;    // standard Windows return value

    if(vertexBuffer == NULL)
        return false;
     
    device3D->SetRenderState(D3DRS_ALPHABLENDENABLE, true); // enable alpha blend

    device3D->SetStreamSource(0, vertexBuffer, 0, sizeof(VertexC));
    device3D->SetFVF(D3DFVF_VERTEX);
    result = device3D->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    device3D->SetRenderState(D3DRS_ALPHABLENDENABLE, false); // alpha blend off
    
    if(FAILED(result))
        return false;

    return true;
}

//----------------------------------------------------------------------------------------------------

void Graphics::DrawSprite(const SpriteData &spriteData, COLOR_ARGB color)
{
	// Draw the sprite described in SpriteData structure.
	// Color is optional, it is applied as a filter, WHITE is default (no change).
	// Creates a sprite Begin/End pair.
	// Pre: spriteData.rect defines the portion of spriteData.texture to draw
	//		spriteData.rect.right must be right edge + 1
	//		spriteData.rect.bottom must be bottom edge + 1

	if(spriteData.texture == NULL)
		return;

	// Find center of sprite
	D3DXVECTOR2 spriteCenter = D3DXVECTOR2((float)(spriteData.width / 2 * spriteData.scale), 
											(float)(spriteData.height / 2 * spriteData.scale));
	// Screen position of the sprite
	D3DXVECTOR2 translate = D3DXVECTOR2((float)spriteData.x, (float)spriteData.y);

	// Scaling X,Y
	D3DXVECTOR2 scaling(spriteData.scale, spriteData.scale);

	if (spriteData.flipHorizontal)
	{
		scaling.x *= -1;

		// Get center of flipped image.
		spriteCenter.x -= (float)(spriteData.width * spriteData.scale);

		// Flip occurs around left edge, translate right to put
		// flipped image in same location as original.
		translate.x += (float)(spriteData.width * spriteData.scale);
	}

	if (spriteData.flipVertical)
	{
		scaling.y *= -1;
		
		// Get center of flipped image
		spriteCenter.y -= (float)(spriteData.height * spriteData.scale);

		// Flip occurs around top edge, translate down to put
		// flipped image in same location as original.
		translate.y += (float)(spriteData.height * spriteData.scale);
	}

	// Create a matrix to rotate, scale and position our sprite
	D3DXMATRIX matrix;
	D3DXMatrixTransformation2D( &matrix,
								NULL,						// keep origin at top left when scaling
								0.0f,						// no scaling rotation
								&scaling,					// scale amount
								&spriteCenter,				// rotation center
								(float)(spriteData.angle),	// rotation angle
								&translate);				// X,Y location

	// Tell the sprite about the matrix
	sprite->SetTransform(&matrix);

	// Draw the sprite
	sprite->Draw(spriteData.texture, &spriteData.rect, NULL, NULL, color);
}

//----------------------------------------------------------------------------------------------------

void Graphics::ChangeDisplayMode(GraphicsNS::DISPLAY_MODE mode)
{
	try
	{
		switch(mode)
		{
		case GraphicsNS::FULLSCREEN:
			if(fullscreen)
				return;
			fullscreen = true;
			break;
		case GraphicsNS::WINDOW:
			if(fullscreen == false)
				return;
			fullscreen = false;
			break;
		default:	// default to toggle window/fullscreen
			fullscreen = !fullscreen;
		}

		Reset();

		if(fullscreen)
		{
			SetWindowLong(hwnd, GWL_STYLE,  WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP);
		}
		else
		{
			SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPos(hwnd, HWND_TOP, 0,0,GAME_WIDTH,GAME_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			// Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
			RECT clientRect;
			GetClientRect(hwnd, &clientRect); // get size of client area of window
			MoveWindow(hwnd,
						0,												// Left
						0,												// Top
						GAME_WIDTH + (GAME_WIDTH-clientRect.right),		// Right
						GAME_HEIGHT + (GAME_HEIGHT-clientRect.bottom),	// Bottom
						TRUE);											// Repaint the window
		}
	}
	catch(...)
	{
		// An error occured, try windowed mode 
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		SetWindowPos(hwnd, HWND_TOP, 0,0,GAME_WIDTH,GAME_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		// Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);   // get size of client area of window
		MoveWindow(hwnd,
					0,												// Left
					0,												// Top
					GAME_WIDTH + (GAME_WIDTH-clientRect.right),		// Right
					GAME_HEIGHT + (GAME_HEIGHT-clientRect.bottom),	// Bottom
					TRUE);											// Repaint the window
	}
}

//----------------------------------------------------------------------------------------------------

HRESULT Graphics::ShowBackBuffer()
{
	// Present the backbuffer to the screen
	result = device3D->Present(NULL, NULL, NULL, NULL);
	return result;
}

//----------------------------------------------------------------------------------------------------

HRESULT Graphics::GetDeviceState()
{ 
	result = E_FAIL; // default to fail, replace on success
	if (device3D == NULL)
		return  result;
	result = device3D->TestCooperativeLevel(); 
	return result;
}

//----------------------------------------------------------------------------------------------------

bool Graphics::IsAdapterCompatible()
{
	// Checks the adapter to see if it is compatible with the BackBuffer height,
	// width and refresh rate specified in d3dpp. Fills in the pMode structure with
	// the format of the compatible mode, if found.
	// Pre: d3dpp is initialized.
	// Post: Returns true if compatible mode found and pMode structure is filled.
	// Returns false if no compatible mode found.

	UINT modes = direct3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, d3DPP.BackBufferFormat);
	for(UINT i=0; i<modes; i++)
	{
		result = direct3D->EnumAdapterModes( D3DADAPTER_DEFAULT, d3DPP.BackBufferFormat, i, &pMode);
		if( pMode.Height == d3DPP.BackBufferHeight &&
			pMode.Width  == d3DPP.BackBufferWidth &&
			pMode.RefreshRate >= d3DPP.FullScreen_RefreshRateInHz)
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------------

HRESULT Graphics::Reset()
{
	InitD3DPP();
	result = device3D->Reset(&d3DPP); // attempt to reset graphics device
	return result;
}

//----------------------------------------------------------------------------------------------------

void Graphics::ReleaseAll()
{
	SafeRelease(device3D);
	SafeRelease(direct3D);
}

//----------------------------------------------------------------------------------------------------

void Graphics::InitD3DPP()
{
	try
	{
		ZeroMemory(&d3DPP, sizeof(d3DPP)); // fills the structure with 0s

		d3DPP.BackBufferWidth = width;
		d3DPP.BackBufferHeight = height;
		if(fullscreen)
			d3DPP.BackBufferFormat = D3DFMT_X8R8G8B8; // 24-bit color
		else
			d3DPP.BackBufferFormat = D3DFMT_UNKNOWN; // use desktop's settings
		d3DPP.BackBufferCount = 1;
		d3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3DPP.hDeviceWindow = hwnd;
		d3DPP.Windowed = (!fullscreen);
		d3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	catch(...)
	{
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error Initializing D3D Presentation Parameters"));
	}
}