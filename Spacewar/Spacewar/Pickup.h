#ifndef _PICKUP_H_
#define _PICKUP_H_
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

namespace PickupNS
{
	const float SPEED = 400.0f;					// 100 pixels per second
}

class Pickup : public Entity
{
public:
	Pickup();

	virtual void Draw();
	virtual bool Initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void Update(float frameTime);
	void Reset();
	void SetGem(bool b)		{ isGem = b; }
	bool IsGem() const		{ return isGem; }

private:
	bool isGem;
};
#endif // _PICKUP_H_