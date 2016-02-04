#include "Pickup.h"

//=============================================================================
// default constructor
//=============================================================================
Pickup::Pickup() : Entity()
{
	isGem = false;
}

//=============================================================================
// Initialize the Pickup.
// Post: returns true if successful, false if failed
//=============================================================================
bool Pickup::Initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	if (Entity::Initialize(gamePtr, width, height, ncols, textureM))
	{
		spriteData.width = textureM->GetWidth();
		spriteData.height = textureM->GetHeight();
		spriteData.rect.bottom = textureM->GetHeight(); // rectangle to select parts of an image
		spriteData.rect.right = textureM->GetWidth();
		velocity.x = 0;
		velocity.y = 0;
		radius = textureM->GetWidth()/2.0f;
		collisionType = EntityNS::CIRCLE;
		active = false;
		visible = false;
		return true;
	}
	return false;
}

//=============================================================================
// draw the Pickup
//=============================================================================
void Pickup::Draw()
{
	Image::Draw();
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Pickup::Update(float frameTime)
{
	if (!active)
		return;

	Entity::Update(frameTime);
	spriteData.x += PickupNS::SPEED * frameTime * velocity.x; // move Pickup along X 

	if (spriteData.x + spriteData.width < 0) // if hit left screen edge
	{
		Reset();
	}
}

void Pickup::Reset()
{
	active = false;
	visible = false;
	spriteData.x = GAME_WIDTH; // position at right screen edge
}