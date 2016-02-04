#include "LevelPlatform.h"

LevelPlatform::LevelPlatform() : Entity()
{
	spriteData.width = LevelPlatformNS::WIDTH;
	spriteData.height = LevelPlatformNS::HEIGHT;
	spriteData.x = LevelPlatformNS::X;
	spriteData.y = LevelPlatformNS::Y;
	spriteData.rect.bottom = LevelPlatformNS::HEIGHT; // rectangle to select parts of an image
	spriteData.rect.right = LevelPlatformNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	collisionType = EntityNS::BOX;
	edge.top = -LevelPlatformNS::HEIGHT/2;		// collision edges
	edge.bottom = LevelPlatformNS::HEIGHT/2;
	edge.left = -LevelPlatformNS::WIDTH/2;
	edge.right = LevelPlatformNS::WIDTH/2;
}

//=============================================================================
// Initialize the platform.
// Post: returns true if successful, false if failed
//=============================================================================
bool LevelPlatform::Initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	return(Entity::Initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the platform
//=============================================================================
void LevelPlatform::Draw()
{
	Image::Draw();
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void LevelPlatform::Update(float frameTime)
{
	Entity::Update(frameTime);
}