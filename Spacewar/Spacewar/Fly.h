#ifndef _FLY_H_
#define _FLY_H_
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

namespace FlyNS
{
	const int WIDTH = 128;						// image width
	const int HEIGHT = 96;						// image height
	const int X = GAME_WIDTH/2 - WIDTH/2;		// location on screen
	const int Y = GAME_HEIGHT/2 - HEIGHT/2;
	const int TEXTURE_COLS = 2;
	const int START_FRAME = 0;
	const int END_FRAME = 1;
	const float ANIMATION_DELAY = 0.5f;			// time between frames
	const float SPEED = 400.0f;					// 100 pixels per second
}

class Fly : public Entity
{
private:
public:
	Fly();

	virtual void Draw();
	virtual bool Initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void Update(float frameTime);
};
#endif // _FLY_H_