// Modified by: Johnny Wu

#ifndef _GAMEPLAYSTATE_H_
#define _GAMEPLAYSTATE_H_
#define WIN32_LEAN_AND_MEAN

#include "Fly.h"
#include "Game.h"
#include "Image.h"
#include "LevelPlatform.h"
#include "Pickup.h"
#include "Player.h"
#include "Spinner.h"
#include "TextureManager.h"
#include "UIElement.h"

class GameplayState : public Game
{
public:
	GameplayState();
	virtual ~GameplayState();

#pragma region Implementation of Base Class Functions

	void Initialize(HWND hwnd);
	void Update();
	void AI();
	void Collisions();
	void Render();
	void ReleaseAll();
	void ResetAll();
	void Restart();
#pragma endregion

private:
	void SetBackgroundImage(int index);
	void ScrollingBackground();
	void SpawnEnemies();
	void SpawnPickups();	

private:
	// Textures
	TextureManager backgroundTextures[2];
	TextureManager platformTexture;
	TextureManager playerTexture;
	TextureManager spinnerTexture;
	TextureManager flyTexture;
	TextureManager pickupTextures[2];
	TextureManager uiTexture;

	// Text
	TextDX* gameOverFont;
	TextDX* replayFont;

	// Background Images
	Image backgroundImages[3];

	// Entities
	LevelPlatform platforms[18];
	Player player;
	Spinner spinners[5];
	Fly	flies [5];
	Pickup pickups[10];
	UIElement playerIcon;
	UIElement coinIcon;
	UIElement gemIcon;
	UIElement hearts[5];
	UIElement coinText[3];
	UIElement gemText[2];

	float enemySpawnTimer;
	float pickupSpawnTimer;
	float timeScale;
	float maxTimeScale;
	int life;
	int coinScore;
	int gemScore;
	bool isPaused;
};

#endif // _GAMEPLAYSTATE_H_