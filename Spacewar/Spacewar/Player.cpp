#include "Player.h"

//=============================================================================
// default constructor
//=============================================================================
Player::Player() : Entity()
{
	// Sprite/Animation
	spriteData.width = PlayerNS::WIDTH;
	spriteData.height = PlayerNS::HEIGHT;
	spriteData.x = PlayerNS::X;
	spriteData.y = PlayerNS::Y;
	spriteData.rect.bottom = PlayerNS::HEIGHT; // rectangle to select parts of an image
	spriteData.rect.right = PlayerNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	frameDelay = PlayerNS::WALK_ANIMATION_DELAY;
	startFrame = PlayerNS::WALK_START_FRAME;
	endFrame = PlayerNS::WALK_END_FRAME;
	currentFrame = startFrame;

	// Collision
	collisionType = EntityNS::ROTATED_BOX;
	edge.top = -PlayerNS::HEIGHT/2 + 8;
	edge.bottom = PlayerNS::HEIGHT/2;
	edge.left = -PlayerNS::WIDTH/2 + 8;
	edge.right = PlayerNS::WIDTH/2 - 8;

	isGrounded = false;
	isWalking = true;
	isJumping = false;
	isDucking = false;
	tookDamage = false;
	isBlinking = false;
	jumpTimer = 0.0f;
	blinkTimer = 0.0f;
	invulTimer = 0.0f;
}

//=============================================================================
// Initialize the Player.
// Post: returns true if successful, false if failed
//=============================================================================
bool Player::Initialize(Game *gamePtr, int width, int height, int ncols,
	TextureManager *textureM)
{
	return(Entity::Initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the Player
//=============================================================================
void Player::Draw()
{
	Image::Draw();
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Player::Update(float frameTime)
{
	CollideWithWall();
	spriteData.x += PlayerNS::WALK_SPEED * frameTime * velocity.x;

	if (!isGrounded)
		spriteData.y += PlayerNS::GRAVITY * frameTime;
	else if (!isWalking && !isDucking)
	{
		isWalking = true;
		startFrame = PlayerNS::WALK_START_FRAME;
		endFrame = PlayerNS::WALK_END_FRAME;
		currentFrame = startFrame;
	}
	if (isJumping)
	{
		jumpTimer += frameTime;
		if (jumpTimer > PlayerNS::JUMP_TIME)
		{
			isJumping = false;
			jumpTimer = 0.0f;
		}
		else if (jumpTimer <= PlayerNS::JUMP_TIME * 0.5f)
			spriteData.y -= PlayerNS::JUMP_SPEED * frameTime;
	}
	if (tookDamage)
	{
		invulTimer += frameTime;
		if (invulTimer > 1.0f)
		{
			invulTimer = 0.0f;
			tookDamage = false;
		}

		if (isBlinking)
			blinkTimer += frameTime;
		if (blinkTimer > 0.3f)
		{
			SetVisible(true);
			blinkTimer = 0.0f;
			isBlinking = false;
			startFrame = PlayerNS::WALK_START_FRAME;
			endFrame = PlayerNS::WALK_END_FRAME;
			currentFrame = startFrame;
		}
		else if (isBlinking)
		{
			SetVisible(!visible);
		}
	}

	Entity::Update(frameTime);
}

void Player::Jump()
{
	if (isGrounded && !isDucking)
	{
		isGrounded = false;
		isJumping = true;
		isWalking = false;
		SetCurrentFrame(PlayerNS::JUMP_FRAME);
		startFrame = currentFrame;
		endFrame = currentFrame;
	}
}

void Player::Duck(bool b)
{
	if (isGrounded && b)
	{
		isDucking = true;
		edge.top = -PlayerNS::WIDTH/2 + 8;
		SetCurrentFrame(PlayerNS::DUCK_FRAME);
		startFrame = currentFrame;
		endFrame = currentFrame;
		isWalking = false;
	}
	else if (isGrounded && !b)
	{
		isDucking = false;
		if (!isWalking)
		{
			isWalking = true;
			edge.top = -PlayerNS::HEIGHT/2 + 8;
			startFrame = PlayerNS::WALK_START_FRAME;
			endFrame = PlayerNS::WALK_END_FRAME;
			currentFrame = startFrame;
		}
	}

}

void Player::TakeDamage()
{
	tookDamage = true;
	isBlinking = true;
	SetCurrentFrame(PlayerNS::HIT_FRAME);
	startFrame = currentFrame;
	endFrame = currentFrame;
}

void Player::CollideWithWall()
{
	if (spriteData.x > GAME_WIDTH - PlayerNS::WIDTH)	// if hit right screen edge
	{
		spriteData.x = GAME_WIDTH - PlayerNS::WIDTH;	// position at right screen edge
	} 
	else if (spriteData.x < 0)							// else if hit left screen edge
	{
		spriteData.x = 0;								// position at left screen edge
	}
}

void Player::ResolveCollision(Entity& ent)
{
	if (GetCenterY() + edge.bottom*GetScale() > ent.GetCenterY() + ent.GetEdge().top*ent.GetScale())
	{
		float diff = (GetCenterY() + edge.bottom*GetScale()) - (ent.GetCenterY() + ent.GetEdge().top * ent.GetScale());
		spriteData.y -= diff;
	}
}