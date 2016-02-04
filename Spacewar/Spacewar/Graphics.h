#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#define WIN32_LEAN_AND_MEAN 

#include <d3d9.h>
#include <d3dx9.h>

#include "Constants.h"
#include "GameError.h"

// DirectX pointer types
#define LP_3D		LPDIRECT3D9
#define LP_3DDEVICE	LPDIRECT3DDEVICE9
#define LP_DXFONT   LPD3DXFONT
#define LP_SPRITE	LPD3DXSPRITE
#define LP_TEXTURE	LPDIRECT3DTEXTURE9
#define VECTOR2		D3DXVECTOR2
#define LP_VERTEXBUFFER LPDIRECT3DVERTEXBUFFER9

// Color defines
// ARGB numbers range from 0 through 255
// Differs from OpenGL, where it range from 0 - 1
#define COLOR_ARGB DWORD
#define SETCOLOR_ARGB(a,r,g,b) \
	((COLOR_ARGB)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

namespace GraphicsNS
{
	// Some common colors
	// ARGB numbers range from 0 through 255
	// A = Alpha channel (transparency where 255 is opaque)
	// R = Red, G = Green, B = Blue
	const COLOR_ARGB ORANGE =	D3DCOLOR_ARGB(255, 255, 165,   0);
	const COLOR_ARGB BROWN =	D3DCOLOR_ARGB(255, 139,  69,  19);
	const COLOR_ARGB LTGRAY =	D3DCOLOR_ARGB(255, 192, 192, 192);
	const COLOR_ARGB GRAY =		D3DCOLOR_ARGB(255, 128, 128, 128);
	const COLOR_ARGB OLIVE =	D3DCOLOR_ARGB(255, 128, 128,   0);
	const COLOR_ARGB PURPLE =	D3DCOLOR_ARGB(255, 128,   0, 128);
	const COLOR_ARGB MAROON =	D3DCOLOR_ARGB(255, 128,   0,   0);
	const COLOR_ARGB TEAL =		D3DCOLOR_ARGB(255,   0, 128, 128);
	const COLOR_ARGB GREEN =	D3DCOLOR_ARGB(255,   0, 128,   0);
	const COLOR_ARGB NAVY =		D3DCOLOR_ARGB(255,   0,   0, 128);
	const COLOR_ARGB WHITE =	D3DCOLOR_ARGB(255, 255, 255, 255);
	const COLOR_ARGB YELLOW =	D3DCOLOR_ARGB(255, 255, 255,   0);
	const COLOR_ARGB MAGENTA =	D3DCOLOR_ARGB(255, 255,   0, 255);
	const COLOR_ARGB RED =		D3DCOLOR_ARGB(255, 255,   0,   0);
	const COLOR_ARGB CYAN =		D3DCOLOR_ARGB(255,   0, 255, 255);
	const COLOR_ARGB LIME =		D3DCOLOR_ARGB(255,   0, 255,   0);
	const COLOR_ARGB BLUE =		D3DCOLOR_ARGB(255,   0,   0, 255);
	const COLOR_ARGB BLACK =	D3DCOLOR_ARGB(255,   0,   0,   0);
	const COLOR_ARGB FILTER =	D3DCOLOR_ARGB(  0,   0,   0,   0);	// use to specify drawing with colorFilter
	const COLOR_ARGB ALPHA25 =	D3DCOLOR_ARGB( 64, 255, 255, 255);	// AND with color to get 25% alpha
	const COLOR_ARGB ALPHA50 =	D3DCOLOR_ARGB(128, 255, 255, 255);	// AND with color to get 50% alpha
	const COLOR_ARGB BACK_COLOR = BLACK;							// background color of game

	enum DISPLAY_MODE{TOGGLE, FULLSCREEN, WINDOW};
}

struct VertexC              // Vertex with Color
{
    float x, y, z;          // vertex location
    float rhw;              // reciprocal homogeneous W (set to 1)
    unsigned long color;    // vertex color
};

// Flexible Vertex Format Codes
// D3DFVF_XYZRHW = The verticies are transformed
// D3DFVF_DIFFUSE = The verticies contain diffuse color data 
#define D3DFVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// SpriteData: The properties required by Graphics::drawSprite to draw a sprite
struct SpriteData
{
	int width;				// width of sprite in pixels
	int height;				// height of sprite in pixels
	float x;				// screen location (top left corner of sprite)
	float y;
	float scale;
	float angle;			// rotation angle in radians
	RECT rect;				// used to select an image from a larger texture
	LP_TEXTURE texture;		// pointer to texture
	bool flipHorizontal;	// true to flip sprite horizontally (mirror)
	bool flipVertical;		// true to flip sprite vertically
};

class Graphics
{
public:
	Graphics();
	virtual ~Graphics();

#pragma region Static Functions
	// Return length of vector v.
	static float Vector2Length(const VECTOR2 *v) {return D3DXVec2Length(v);}

	// Return Dot product of vectors v1 and v2.
	static float Vector2Dot(const VECTOR2 *v1, const VECTOR2 *v2) {return D3DXVec2Dot(v1, v2);}

	// Normalize vector v.
	static void Vector2Normalize(VECTOR2 *v) {D3DXVec2Normalize(v, v);}

	// Transform vector v with matrix m.
	static VECTOR2* Vector2Transform(VECTOR2 *v, D3DXMATRIX *m) {return D3DXVec2TransformCoord(v,v,m);}
#pragma endregion

#pragma region Member Functions

	void Initialize(HWND hw, int width, int height, bool fullscreen);	// initialize directX graphics

	HRESULT BeginScene()
	{
		result = E_FAIL;
		if(device3D == NULL)
			return result;

		// Clear backbuffer to backColor
		device3D->Clear(0, NULL, D3DCLEAR_TARGET, backColor, 1.0F, 0);
		result = device3D->BeginScene(); // begin scene for drawing
		return result;
	}
	HRESULT EndScene()
	{
		result = E_FAIL;
		if(device3D)
			result = device3D->EndScene();
		return result;
	}

	HRESULT LoadTexture(const char * filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture);
	void DrawSprite(const SpriteData &spriteData, COLOR_ARGB color = GraphicsNS::WHITE); // default to white color filter (no change)
	
	void SpriteBegin()
	{
		sprite->Begin(D3DXSPRITE_ALPHABLEND);
	}

	void SpriteEnd() 
	{
		sprite->End();
	}

	void ChangeDisplayMode(GraphicsNS::DISPLAY_MODE mode = GraphicsNS::TOGGLE);
	void SetBackColor(COLOR_ARGB c) { backColor = c; }	// set color used to clear screen
	HRESULT ShowBackBuffer();	// display the offscreen backbuffer to the screen
	HRESULT GetDeviceState();	// test for lost device
	bool IsAdapterCompatible();
    HRESULT CreateVertexBuffer(VertexC verts[], UINT size, LP_VERTEXBUFFER &vertexBuffer);
    bool DrawQuad(LP_VERTEXBUFFER vertexBuffer);
	HRESULT LoadTextureSystemMem(const char *filename, COLOR_ARGB transcolor, UINT &width, UINT &height, LP_TEXTURE &texture);
	HRESULT Reset();			// reset the graphics device.
	void ReleaseAll();			// releases direct3d and device3d
#pragma endregion

#pragma region Accessors/Mutators
	LP_3D GetD3D()				{ return direct3D; }
	LP_3DDEVICE Get3Ddevice()	{ return device3D; }
	HDC GetDeviceContext()		{ return GetDC(hwnd); }
	LP_SPRITE GetSprite()		{ return sprite; }
	bool GetFullscreen()		{ return fullscreen; }
#pragma endregion

private:
	// DirectX pointers and stuff
	LP_3D					direct3D;
	LP_3DDEVICE				device3D;
	LP_SPRITE				sprite;
	D3DPRESENT_PARAMETERS	d3DPP;
	D3DDISPLAYMODE			pMode;

	// Other variables
	HRESULT		result;
	HWND		hwnd;
	bool		fullscreen;
	int			width;
	int			height;
	COLOR_ARGB	backColor; // background color

	void InitD3DPP();	// intialize d3D Presentation Parameters

};
#endif // _GRAPHICS_H_