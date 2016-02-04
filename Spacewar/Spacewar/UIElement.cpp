#include "UIElement.h"

UIElement::UIElement() 
	: Entity()
{
}

//=============================================================================
// Initialize the UI.
// Post: returns true if successful, false if failed
//=============================================================================
bool UIElement::Initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	spriteData.width = width;
	spriteData.height = height;
	spriteData.x = GAME_WIDTH/2 - width/2;
	spriteData.y = GAME_HEIGHT/2 - height/2;
	spriteData.rect.bottom = height; // rectangle to select parts of an image
	spriteData.rect.right = width;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = 0;
	endFrame = 0;
	return(Entity::Initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the UI
//=============================================================================
void UIElement::Draw()
{
	Image::Draw();
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void UIElement::Update(float frameTime)
{
	Entity::Update(frameTime);
}