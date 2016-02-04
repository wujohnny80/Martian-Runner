#ifndef _LEVELPLATFORM_H_
#define _LEVELPLATFORM_H_
#define WIN32_LEAN_AND_MEAN

#include "Entity.h"
#include "Constants.h"

namespace LevelPlatformNS
{
	const int WIDTH = 128;					// image width
	const int HEIGHT = 128;					// image height
	const int X = GAME_WIDTH/2 - WIDTH/2;	// location on screen
	const int Y = GAME_HEIGHT/2 - HEIGHT/2;
}

class LevelPlatform : public Entity
{
public:
	LevelPlatform();

	virtual void Draw();
	virtual bool Initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void Update(float frameTime);

private:
};
#endif // _LEVELPLATFORM_H_