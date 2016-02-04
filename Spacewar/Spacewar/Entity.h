#ifndef _ENTITY_H_
#define _ENTITY_H_
#define WIN32_LEAN_AND_MEAN

#include "Game.h"
#include "Image.h"
#include "Input.h"

namespace EntityNS
{
	enum COLLISION_TYPE {NONE, CIRCLE, BOX, ROTATED_BOX};
	const float GRAVITY = 6.67428e-11f;				// gravitational constant
}

class Entity : public Image
{
	// Entity properties
protected:
	EntityNS::COLLISION_TYPE collisionType;
	VECTOR2 center;         // center of entity
	float   radius;         // radius of collision circle
	VECTOR2 distSquared;    // used for calculating circle collision
	float   sumRadiiSquared;
	// edge specifies the collision box relative to the center of the entity.
	// left and top are typically negative numbers
	RECT    edge;           // for BOX and ROTATED_BOX collision detection
	VECTOR2 corners[4];     // for ROTATED_BOX collision detection
	VECTOR2 edge01,edge03;  // edges used for projection
	float   edge01Min, edge01Max, edge03Min, edge03Max; // min and max projections
	VECTOR2 velocity;       // velocity
	VECTOR2 deltaV;         // added to velocity during next call to update()
	float   mass;           // Mass of entity
	float   health;         // health 0 to 100
	float   rr;             // Radius squared variable
	float   force;          // Force of gravity
	float   gravity;        // gravitational constant of the game universe
	Input   *input;         // pointer to the input system
	HRESULT hr;             // standard return type
	bool    active;         // only active entities may collide
	bool    rotatedBoxReady;    // true when rotated collision box is ready

	// --- The following functions are protected because they are not intended to be
	// --- called from outside the class.
	// Circular collision detection 
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideCircle(Entity &ent, VECTOR2 &collisionVector);
	// Axis aligned box collision detection
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideBox(Entity &ent, VECTOR2 &collisionVector);
	// Separating axis collision detection between boxes
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideRotatedBox(Entity &ent, VECTOR2 &collisionVector);
	// Separating axis collision detection between box and circle
	// Pre: &ent = Other entity
	// Post: &collisionVector contains collision vector
	virtual bool collideRotatedBoxCircle(Entity &ent, VECTOR2 &collisionVector);
	// Separating axis collision detection helper functions
	void computeRotatedBox();
	bool projectionsOverlap(Entity &ent);
	bool collideCornerCircle(VECTOR2 corner, Entity &ent, VECTOR2 &collisionVector);

public:
	Entity();
	virtual ~Entity() {}

#pragma region Accessors/Mutators
	virtual const VECTOR2* GetCenter()
	{
		center = VECTOR2(GetCenterX(), GetCenterY());
		return &center;
	}
	virtual float GetRadius() const     {return radius;}
	virtual const RECT& GetEdge() const {return edge;}
	virtual const VECTOR2* GetCorner(UINT c) const
	{
		if(c>=4) 
			c=0;
		return &corners[c]; 
	}
	virtual const VECTOR2 GetVelocity() const {return velocity;}
	virtual bool  GetActive()         const {return active;}
	virtual float GetMass()           const {return mass;}
	virtual float GetGravity()        const {return gravity;}
	virtual float GetHealth()         const {return health;}
	virtual EntityNS::COLLISION_TYPE GetCollisionType() {return collisionType;}

	virtual void SetVelocity(VECTOR2 v)    {velocity = v;}
	virtual void SetDeltaV(VECTOR2 dv)     {deltaV = dv;}
	virtual void SetActive(bool a)         {active = a;}
	virtual void SetHealth(float h)         {health = h;}
	virtual void SetMass(float m)          {mass = m;}
	virtual void SetGravity(float g)       {gravity = g;}
	virtual void SetCollisionRadius(float r)    {radius = r;}
#pragma endregion

	virtual void Update(float frameTime);
	virtual bool Initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	virtual void Activate();
	virtual void AI(float frameTime, Entity &ent);
	virtual bool OutsideRect(RECT rect);
	virtual bool CollidesWith(Entity &ent, VECTOR2 &collisionVector);
	virtual void Damage(int weapon);
	void Bounce(VECTOR2 &collisionVector, Entity &ent);
	void GravityForce(Entity *other, float frameTime);
};

#endif // _ENTITY_H_