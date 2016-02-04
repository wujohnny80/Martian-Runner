#include "Fly.h"

//=============================================================================
// default constructor
//=============================================================================
Fly::Fly() : Entity()
{
	spriteData.width = FlyNS::WIDTH;
	spriteData.height = FlyNS::HEIGHT;
	spriteData.x = FlyNS::X;
	spriteData.y = FlyNS::Y;
	spriteData.rect.bottom = FlyNS::HEIGHT; // rectangle to select parts of an image
	spriteData.rect.right = FlyNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	frameDelay = FlyNS::ANIMATION_DELAY;
	startFrame = FlyNS::START_FRAME;
	endFrame = FlyNS::END_FRAME;
	currentFrame = startFrame;
	radius = FlyNS::HEIGHT/2.0f;
	collisionType = EntityNS::CIRCLE;
	active = false;
	visible = false;
}

//=============================================================================
// Initialize the Fly.
// Post: returns true if successful, false if failed
//=============================================================================
bool Fly::Initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	return(Entity::Initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the Fly
//=============================================================================
void Fly::Draw()
{
	Image::Draw();
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Fly::Update(float frameTime)
{
	if (!active)
		return;

	Entity::Update(frameTime);
	spriteData.x += FlyNS::SPEED * frameTime * velocity.x; // move Fly along X 

	if (spriteData.x + spriteData.width < 0) // if hit left screen edge
	{
		active = false;
		visible = false;
		spriteData.x = GAME_WIDTH; // position at right screen edge
	}
}