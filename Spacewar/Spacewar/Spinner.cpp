#include "Spinner.h"

//=============================================================================
// default constructor
//=============================================================================
Spinner::Spinner() : Entity()
{
	spriteData.width = SpinnerNS::WIDTH;
	spriteData.height = SpinnerNS::HEIGHT;
	spriteData.x = SpinnerNS::X;
	spriteData.y = SpinnerNS::Y;
	spriteData.rect.bottom = SpinnerNS::HEIGHT; // rectangle to select parts of an image
	spriteData.rect.right = SpinnerNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	frameDelay = SpinnerNS::ANIMATION_DELAY;
	startFrame = SpinnerNS::START_FRAME;
	endFrame = SpinnerNS::END_FRAME;
	currentFrame = startFrame;
	radius = SpinnerNS::WIDTH/2.0f;
	collisionType = EntityNS::CIRCLE;
	active = false;
	visible = false;
}

//=============================================================================
// Initialize the Spinner.
// Post: returns true if successful, false if failed
//=============================================================================
bool Spinner::Initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	return(Entity::Initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the Spinner
//=============================================================================
void Spinner::Draw()
{
	Image::Draw();
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Spinner::Update(float frameTime)
{
	if (!active)
		return;

	Entity::Update(frameTime);
	spriteData.x += SpinnerNS::SPEED * frameTime * velocity.x; // move Spinner along X 

	if (spriteData.x + spriteData.width < 0) // if hit left screen edge
	{
		active = false;
		visible = false;
		spriteData.x = GAME_WIDTH; // position at right screen edge
	}
}