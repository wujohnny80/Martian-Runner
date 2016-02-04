#ifndef _IMAGE_H_
#define _IMAGE_H_
#define WIN32_LEAN_AND_MEAN

#include "Constants.h"
#include "TextureManager.h"

class Image
{
public:
	Image();
	virtual ~Image();

#pragma region Accessors/Mutators

	const virtual SpriteData& GetSpriteInfo()	{ return spriteData; }
	virtual bool GetVisible()					{ return visible; }
	virtual float GetX()						{ return spriteData.x; }
	virtual float GetY()						{ return spriteData.y; }
	virtual float GetScale()					{ return spriteData.scale; }
	virtual int GetWidth()						{ return spriteData.width; }
	virtual int GetHeight()						{ return spriteData.height; }
	virtual float GetCenterX()					{ return spriteData.x + spriteData.width / 2 * spriteData.scale; }
	virtual float GetCenterY()					{ return spriteData.y + spriteData.height / 2 * spriteData.scale; }
	virtual float GetDegrees()					{ return spriteData.angle * (180.0f / (float)PI); }
	virtual float GetRadians()					{ return spriteData.angle; }
	virtual float GetFrameDelay()				{ return frameDelay; }
	virtual int GetStartFrame()					{ return startFrame; }
	virtual int GetEndFrame()					{ return endFrame; }
	virtual int GetCurrentFrame()				{ return currentFrame; }
	virtual RECT GetSpriteDataRect()			{ return spriteData.rect; }
	virtual bool GetAnimationComplete()			{ return animComplete; }
	virtual COLOR_ARGB GetColorFilter()			{ return colorFilter; }

	virtual void SetX(float newX)				{ spriteData.x = newX; }
	virtual void SetY(float newY)				{ spriteData.y = newY; }
	virtual void SetScale(float s)				{ spriteData.scale = s; }
	virtual void SetDegrees(float deg)			{ spriteData.angle = deg * ((float)PI/180.0f); }
	virtual void SetRadians(float rad)			{ spriteData.angle = rad; }
	virtual void SetVisible(bool v)				{ visible = v; }
	virtual void SetFrameDelay(float d)			{ frameDelay = d; }
	virtual void SetFrames(int s, int e)		{ startFrame = s; endFrame = e; }

	virtual void SetCurrentFrame(int c);
	virtual void SetRect();
	virtual void SetSpriteDataRect(RECT r)						{ spriteData.rect = r; }
	virtual void SetLoop(bool lp)								{ loop = lp; }
	virtual void SetAnimationComplete(bool a)					{ animComplete = a; }
	virtual void SetColorFilter(COLOR_ARGB color)				{ colorFilter = color; }
	virtual void SetTextureManager(TextureManager *textureM)	{ textureManager = textureM; }

#pragma endregion

	virtual bool Initialize(Graphics *g, int width, int height, int ncols, TextureManager *textureM);
	virtual void Draw(COLOR_ARGB color = GraphicsNS::WHITE);
	virtual void Draw(SpriteData sd, COLOR_ARGB color = GraphicsNS::WHITE);
	virtual void Update(float frameTime);
	virtual void FlipHorizontal(bool flip)		{ spriteData.flipHorizontal = flip; }
	virtual void FlipVertical(bool flip)		{ spriteData.flipVertical = flip; }

protected:
	Graphics *graphics;
	TextureManager *textureManager;

	SpriteData spriteData;	// SpriteData is defined in "graphics.h"
	COLOR_ARGB colorFilter;	// applied as a color filter (use WHITE for no change)
	int cols;				// number of cols (1 to n) in multi-frame sprite
	int startFrame;			// first frame of current animation
	int endFrame;			// end frame of current animation
	int currentFrame;		// current frame of animation
	float frameDelay;		// how long between frames of animation
	float animTimer;		// animation timer
	HRESULT hr;				// standard return type
	bool loop;				// true to loop frames
	bool visible;			// true when visible
	bool initialized;		// true when successfully initialized
	bool animComplete;		// true when loop is false and endFrame has finished displaying
};

#endif // _IMAGE_H_