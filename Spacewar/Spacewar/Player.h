#ifndef _PLAYER_H_
#define _PLAYER_H_
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

namespace PlayerNS
{
	const int WIDTH = 120;						// image width
	const int HEIGHT = 160;						// image height
	const int X = GAME_WIDTH/2 - WIDTH/2;		// location on screen
	const int Y = GAME_HEIGHT/2 - HEIGHT/2;
	const int TEXTURE_COLS = 6;					// texture has 6 columns
	const int WALK_START_FRAME = 0;
	const int WALK_END_FRAME = 9;
	const int JUMP_FRAME = 10;
	const int HIT_FRAME = 11;
	const int DUCK_FRAME = 12;
	const int EMPTY_FRAME = 13;
	const float WALK_ANIMATION_DELAY = 0.06f;	// time between frames

	const float WALK_SPEED = 200.0f;
	const float JUMP_TIME = 0.7f;
	const float JUMP_SPEED = 1200.0f;
	const float GRAVITY = 600.0f;
}

class Player : public Entity
{
public:
	Player();

	virtual void Draw();
	virtual bool Initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void Update(float frameTime);

	bool TookDamage() const			{ return tookDamage; }
	void SetGrounded(bool b)		{ isGrounded = b; }
	void Jump();
	void Duck(bool b);
	void TakeDamage();
	void ResolveCollision(Entity& ent);

private:
	void CollideWithWall();
private:
	bool isGrounded;
	bool isWalking;
	bool isJumping;
	bool isDucking;
	bool tookDamage;
	bool isBlinking;
	float jumpTimer;
	float blinkTimer;
	float invulTimer;
};
#endif // _PLAYER_H_