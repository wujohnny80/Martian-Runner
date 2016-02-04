#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_
#define WIN32_LEAN_AND_MEAN

#include "Entity.h"
#include "Constants.h"

class UIElement : public Entity
{
public:
	UIElement();

	virtual void Draw();
	virtual bool Initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void Update(float frameTime);

private:
};
#endif // _UIELEMENT_H_