// Modified by : Johnny Wu

#include <stdlib.h>
#include <time.h>

#include "GameplayState.h"

GameplayState::GameplayState()
{
	gameOverFont = new TextDX();
	replayFont = new TextDX();
	srand((unsigned int)time(NULL));
	enemySpawnTimer = 0.0f;
	pickupSpawnTimer = 0.0f;
	timeScale = 1.0f;
	maxTimeScale = 3.0f;
	life = 5;
	coinScore = 0;
	gemScore = 0;
	isPaused = false;
}

//----------------------------------------------------------------------------------------------------

GameplayState::~GameplayState()
{
	ReleaseAll(); // call onLostDevice() for every graphics item
	SAFE_DELETE(gameOverFont);
	SAFE_DELETE(replayFont);
}

//----------------------------------------------------------------------------------------------------

void GameplayState::Initialize(HWND hwnd)
{
	Game::Initialize(hwnd);

	 // Text
	if(gameOverFont->initialize(graphics, 96, false, false, "Arial") == false)
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing DirectX font"));
	if(replayFont->initialize(graphics, 72, false, false, "Arial") == false)
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing DirectX font"));

	// Textures
	// Background
	if (!backgroundTextures[0].Initialize(graphics, "./Assets/Background/uncolored_forest.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing uncolored_forest.png"));
	if (!backgroundTextures[1].Initialize(graphics, "./Assets/Background/uncolored_plain.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing uncolored_plain.png"));
	// Platform
	if (!platformTexture.Initialize(graphics, "./Assets/Platforms/grassMid.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing grassMid.png"));
	// Player
	if (!playerTexture.Initialize(graphics, "./Assets/Player/player_red.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing player_red.png"));	
	// Enemies
	if (!spinnerTexture.Initialize(graphics, "./Assets/Enemies/spinnerHalf.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing spinner.png"));
	if (!flyTexture.Initialize(graphics, "./Assets/Enemies/fly.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing fly.png"));
	// Pickups
	if (!pickupTextures[0].Initialize(graphics, "./Assets/Items/coinGold.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing coinGold.png"));
	if (!pickupTextures[1].Initialize(graphics, "./Assets/Items/gemBlue.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing gemBlue.png"));
	// UI
	if (!uiTexture.Initialize(graphics, "./Assets/HUD/hud.png"))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing hud.png"));

	// Initialize Background/Platform Images
	for (int i = 0; i < 3; ++i)
	{
		SetBackgroundImage(i);
		// position them
		if (i > 0)
			backgroundImages[i].SetX(backgroundImages[i-1].GetX() + backgroundImages[i-1].GetWidth() * backgroundImages[i-1].GetScale());
	}
	for (int i = 0; i < 18; ++i)
	{
		if (!platforms[i].Initialize(this, 0, 0, 0, &platformTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing platforms"));
		platforms[i].SetX(i * platforms[i].GetWidth());
		platforms[i].SetY(GAME_HEIGHT - platforms[i].GetHeight());
	}

	// Initialize Entities
	// Player
	if (!player.Initialize(this, PlayerNS::WIDTH, PlayerNS::HEIGHT, PlayerNS::TEXTURE_COLS, &playerTexture))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing player"));
	player.SetFrames(PlayerNS::WALK_START_FRAME, PlayerNS::WALK_END_FRAME);
	player.SetCurrentFrame(PlayerNS::WALK_START_FRAME);
	player.SetX(GAME_WIDTH / 3);
	player.SetY(GAME_HEIGHT / 2);

	// Spinners
	for (int i = 0; i < 5; ++i)
	{
		if (!spinners[i].Initialize(this, SpinnerNS::WIDTH, SpinnerNS::HEIGHT, SpinnerNS::TEXTURE_COLS, &spinnerTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing spinner"));
		spinners[i].SetX(GAME_WIDTH);
		spinners[i].SetY(GAME_HEIGHT - platforms[0].GetHeight() - spinners[i].GetHeight() / 2);
	}
	
	// Flies
	for (int i = 0; i < 5; ++i)
	{
		if (!flies[i].Initialize(this, FlyNS::WIDTH, FlyNS::HEIGHT, FlyNS::TEXTURE_COLS, &flyTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing spinner"));
		flies[i].SetX(GAME_WIDTH);
		flies[i].SetY(GAME_HEIGHT - platforms[0].GetHeight() - flies[i].GetHeight() - player.GetWidth() - 16);
	}

	// Coins
	for (int i = 0; i < 10; ++i)
	{
		if (!pickups[i].Initialize(this, 0, 0, 0, &pickupTextures[0]))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing pickup image"));
		pickups[i].SetX(GAME_WIDTH);
	}

	// Initialize UI elements
	// Player icon
	if (!playerIcon.Initialize(this, 64, 64, 5, &uiTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing playerIcon"));
	playerIcon.SetCurrentFrame(14);
	playerIcon.SetX(32);
	playerIcon.SetY(32);

	// Hearts
	for (int i = 0; i < 5; ++i)
	{
		if (!hearts[i].Initialize(this, 64, 64, 5, &uiTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing hearts"));
		hearts[i].SetCurrentFrame(13);
		hearts[i].SetX( (i * hearts[i].GetWidth() +  64) + playerIcon.GetX() + 16);
		hearts[i].SetY(32);
	}

	// Coin icon
	if (!coinIcon.Initialize(this, 64, 64, 5, &uiTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing coinIcon"));
	coinIcon.SetCurrentFrame(10);
	coinIcon.SetX(hearts[4].GetX() + hearts[4].GetWidth() + 128);
	coinIcon.SetY(32);

	// coinText
	for (int i = 0; i < 3; ++i)
	{
		if (!coinText[i].Initialize(this, 64, 64, 5, &uiTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing coinText"));
		coinText[i].SetCurrentFrame(0);
		coinText[i].SetX((i * coinText[i].GetWidth() + 64) + coinIcon.GetX() + 32);
		coinText[i].SetY(32);
	}

	// Gem icon
	if (!gemIcon.Initialize(this, 64, 64, 5, &uiTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing gemIcon"));
	gemIcon.SetCurrentFrame(11);
	gemIcon.SetX(coinText[2].GetX() + coinText[2].GetWidth() + 128);
	gemIcon.SetY(32);

	// gemText
	for (int i = 0; i < 2; ++i)
	{
		if (!gemText[i].Initialize(this, 64, 64, 5, &uiTexture))
			throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing gemText"));
		gemText[i].SetCurrentFrame(0);
		gemText[i].SetX((i * gemText[i].GetWidth() + 64) + gemIcon.GetX() + 32);
		gemText[i].SetY(32);
	}
}

//----------------------------------------------------------------------------------------------------

void GameplayState::Update()
{
	if (isPaused)
	{
		if (input->WasKeyPressed(Key::R))
			Restart();
	}
	else
	{
		timeScale += frameTime * 0.01f;
		timeScale = Clamp(timeScale, 1.0f, maxTimeScale);
		enemySpawnTimer += frameTime;
		pickupSpawnTimer += frameTime;

		ScrollingBackground();
		player.SetVelocity(VECTOR2(0.0f, 0.0f));

		// Get movement input
		if (input->IsKeyDown(Key::RIGHT_ARROW))
		{
			player.SetVelocity(VECTOR2(1.0f * timeScale, 0.0f));
		}
		if (input->IsKeyDown(Key::LEFT_ARROW))
		{
			player.SetVelocity(VECTOR2(-1.0f * timeScale, 0.0f));
		}
		if (input->IsKeyDown(Key::UP_ARROW))
		{
			player.Jump();
		}
		if (input->IsKeyDown(Key::DOWN_ARROW))
		{
			player.Duck(true);
		}
		else 
		{
			player.Duck(false);
		}

		// Update player
		player.Update(frameTime);
	
		// Update enemies
		SpawnEnemies();

		// Update pickups
		SpawnPickups();

		// Update UI
		playerIcon.Update(frameTime);
		coinIcon.Update(frameTime);
		gemIcon.Update(frameTime);
		for (int i = 0; i < 5; ++i)
		{
			hearts[i].Update(frameTime);
		}
		for (int i = 0; i < 3; ++i)
		{
			coinText[i].Update(frameTime);
		}
		for (int i = 0; i < 2; ++i)
		{
			gemText[i].Update(frameTime);
		}
	}
}

//----------------------------------------------------------------------------------------------------

void GameplayState::AI()
{
}

//----------------------------------------------------------------------------------------------------

void GameplayState::Collisions()
{
	if (isPaused)
		return;

	VECTOR2 collisionVector;
	// collision between players and platforms
	for (int i = 0; i < 18; ++i)
	{
		if (player.CollidesWith(platforms[i], collisionVector))
		{
			player.ResolveCollision(platforms[i]);
			player.SetGrounded(true);
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		if (!player.TookDamage())
		{
			if (player.CollidesWith(spinners[i], collisionVector) || player.CollidesWith(flies[i], collisionVector))
			{
				player.TakeDamage();
				timeScale = 1.0f;
				life--;
				if (life > 0 && life < 5)
					hearts[life].SetCurrentFrame(12);
				else
				{
					hearts[0].SetCurrentFrame(12);
					isPaused = true;
				}
			}
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			if (pickups[i].CollidesWith(spinners[j], collisionVector) || pickups[i].CollidesWith(flies[j], collisionVector))
			{
				// Spawned on top of an enemy
				pickups[i].Reset();
			}
		}
		if (player.CollidesWith(pickups[i], collisionVector))
		{
			// collided with player
			if (pickups[i].IsGem())
			{
				gemScore ++;
				gemScore = Clamp(gemScore, 0, 99);
				if (gemScore > 9)
				{
					gemText[0].SetCurrentFrame(gemScore / 10);
					gemText[1].SetCurrentFrame(gemScore % 10);
				}
				else
					gemText[1].SetCurrentFrame(gemScore);
			}
			else
			{
				coinScore ++;
				coinScore = Clamp(coinScore, 0, 999);
				if (coinScore > 99)
				{
					coinText[0].SetCurrentFrame(coinScore / 100);
					coinText[1].SetCurrentFrame(coinScore % 100 / 10);
					coinText[2].SetCurrentFrame(coinScore % 10);
				}
				else if (coinScore > 9)
				{
					coinText[1].SetCurrentFrame(coinScore / 10);
					coinText[2].SetCurrentFrame(coinScore % 10);
				}
				else
				{
					coinText[2].SetCurrentFrame(coinScore);
				}
			}

			pickups[i].Reset();
		}
	}
}

//----------------------------------------------------------------------------------------------------

void GameplayState::Render()
{
	graphics->SpriteBegin();
	// Draw background/Platforms
	for (int i = 0; i < 3; ++i)
	{
		backgroundImages[i].Draw();
	}
	
	for (int i = 0; i < 18; ++i)
	{
		platforms[i].Draw();
	}

	// Draw players
	player.Draw();

	// Draw enemies
	for (int i = 0; i < 5; ++i)
	{
		spinners[i].Draw();
	}

	for (int i = 0; i < 5; ++i)
	{
		flies[i].Draw();
	}

	// Draw pickups
	for (int i = 0; i < 10; ++i)
	{
		pickups[i].Draw();
	}
	
	// Draw UI
	playerIcon.Draw();
	coinIcon.Draw();
	gemIcon.Draw();
	for (int i = 0; i < 5; ++i)
	{
		hearts[i].Draw();
	}
	for (int i = 0; i < 3; ++i)
	{
		coinText[i].Draw();
	}
	for (int i = 0; i < 2; ++i)
	{
		gemText[i].Draw();
	}

	if (isPaused)
	{
		// DirectX text heading
		gameOverFont->setFontColor(SETCOLOR_ARGB(255,255,0,0));
		gameOverFont->print("Game Over", GAME_WIDTH / 2 - 200, GAME_HEIGHT / 2 - 100);
		replayFont->setFontColor(SETCOLOR_ARGB(255,128,128,128));
		replayFont->print("Press R to replay", GAME_WIDTH / 2 - 225, GAME_HEIGHT / 2);
	}

	graphics->SpriteEnd();
}

//----------------------------------------------------------------------------------------------------

void GameplayState::SetBackgroundImage(int index)
{
	int rnd = rand() % 2;
	if (!backgroundImages[index].Initialize(graphics, 0, 0, 0, &backgroundTextures[rnd]))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing background image"));
	// Rescale background to fit screen
		backgroundImages[index].SetScale(0.88f); // 900/1024 window height divided by image height
}

//----------------------------------------------------------------------------------------------------

void GameplayState::ScrollingBackground()
{
	for (int i = 0; i < 3; ++i)
	{
		// Check out of screen
		if (backgroundImages[i].GetX() + backgroundImages[i].GetWidth() < 0) // if outside left screen edge
		{
			// Pick a random background texture
			SetBackgroundImage(i);
			// position at right of the last image
			if (i == 0)
				backgroundImages[i].SetX(backgroundImages[2].GetX() + backgroundImages[2].GetWidth() * backgroundImages[2].GetScale());
			else
				backgroundImages[i].SetX(backgroundImages[i-1].GetX() + backgroundImages[i-1].GetWidth() * backgroundImages[i-1].GetScale());
		}

		// Move slowly left
		backgroundImages[i].SetX(backgroundImages[i].GetX() - 50.0f * frameTime * timeScale);
	}

	for (int i = 0; i < 18; ++i)
	{
		// Check out of screen
		if (platforms[i].GetX() + platforms[i].GetWidth() < 0) // if outside left screen edge
		{
			// position at right of the last image
			if (i == 0)
				platforms[i].SetX(platforms[17].GetX() + platforms[2].GetWidth());
			else
				platforms[i].SetX(platforms[i-1].GetX() + platforms[i-1].GetWidth());
		}

		// Move slowly left
		platforms[i].SetX(platforms[i].GetX() - PickupNS::SPEED * frameTime * timeScale);
	}
}

//----------------------------------------------------------------------------------------------------

void GameplayState::SpawnEnemies()
{	
	int rnd = rand() % 2;
	for (int i = 0; i < 5; ++i)
	{
		if (rnd == 0)
		{
			if (!spinners[i].GetActive() && enemySpawnTimer > 3.0f / timeScale)
			{
				enemySpawnTimer = 0.0f;
				spinners[i].Activate();
				spinners[i].SetVisible(true);
			}
		}
		else
		{
			if (!flies[i].GetActive() && enemySpawnTimer > 3.0f / timeScale)
			{
				enemySpawnTimer = 0.0f;
				flies[i].Activate();
				flies[i].SetVisible(true);
			}
		}
		spinners[i].SetVelocity(VECTOR2(-1.0f * timeScale, 0.0f));
		spinners[i].Update(frameTime);
		flies[i].SetVelocity(VECTOR2(-1.0f * timeScale, 0.0f));
		flies[i].Update(frameTime);
	}
}

//----------------------------------------------------------------------------------------------------

void GameplayState::SpawnPickups()
{
	int rnd = rand() % 2;
	for(int i = 0; i < 10; ++i)
	{
		// Initialize and activate 
		if (!pickups[i].GetActive() && pickupSpawnTimer > 1.5f / timeScale)
		{
			// Spawn gems with 1% chance
			if (RandomFloat(0.0f, 1.0f) <= 0.05f)
			{
				if (!pickups[i].Initialize(this, 0, 0, 0, &pickupTextures[1]))
					throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing pickup image"));
				pickups[i].SetGem(true);
			}
			else
			{
				if (!pickups[i].Initialize(this, 0, 0, 0, &pickupTextures[0]))
					throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing pickup image"));
				pickups[i].SetGem(false);
			}

			// Set Y position
			if (rnd == 0)
				pickups[i].SetY(GAME_HEIGHT - platforms[0].GetHeight() - pickups[i].GetHeight() - 32);
			else
				pickups[i].SetY(GAME_HEIGHT - platforms[0].GetHeight() - pickups[i].GetHeight() - player.GetWidth() - 48);

			pickupSpawnTimer = 0.0f;
			pickups[i].Activate();
			pickups[i].SetVisible(true);
		}
		
		pickups[i].SetVelocity(VECTOR2(-1.0f * timeScale, 0.0f));
		pickups[i].Update(frameTime);
	}
}


//----------------------------------------------------------------------------------------------------

void GameplayState::Restart()
{
	// Reset Entities
	// Player
	player.SetX(GAME_WIDTH / 3);
	player.SetY(GAME_HEIGHT / 2);
	player.SetGrounded(false);
	player.Update(frameTime);

	// Spinners
	for (int i = 0; i < 5; ++i)
	{
		spinners[i].SetActive(false);
		spinners[i].SetVisible(false);
		spinners[i].SetX(GAME_WIDTH);
		spinners[i].SetY(GAME_HEIGHT - platforms[0].GetHeight() - spinners[i].GetHeight() / 2);
	}
	
	// Flies
	for (int i = 0; i < 5; ++i)
	{
		flies[i].SetActive(false);
		flies[i].SetVisible(false);
		flies[i].SetX(GAME_WIDTH);
		flies[i].SetY(GAME_HEIGHT - platforms[0].GetHeight() - flies[i].GetHeight() - player.GetWidth() - 16);
	}

	// Coins
	for (int i = 0; i < 10; ++i)
	{
		pickups[i].SetActive(false);
		pickups[i].SetVisible(false);
		pickups[i].SetX(GAME_WIDTH);
	}

	// Initialize UI elements
	// Hearts
	for (int i = 0; i < 5; ++i)
	{
		hearts[i].SetCurrentFrame(13);
	}

	// coinText
	for (int i = 0; i < 3; ++i)
	{
		coinText[i].SetCurrentFrame(0);
	}

	// gemText
	for (int i = 0; i < 2; ++i)
	{
		gemText[i].SetCurrentFrame(0);
	}

	enemySpawnTimer = 0.0f;
	pickupSpawnTimer = 0.0f;
	timeScale = 1.0f;
	maxTimeScale = 3.3f;
	life = 5;
	coinScore = 0;
	gemScore = 0;
	isPaused = false;
}

//----------------------------------------------------------------------------------------------------

void GameplayState::ReleaseAll()
{
	for (int i = 0; i < 2; ++i)
	{
		backgroundTextures[i].OnLostDevice();
	}
	platformTexture.OnLostDevice();
	playerTexture.OnLostDevice();
	spinnerTexture.OnLostDevice();
	flyTexture.OnLostDevice();

	for (int i = 0; i < 2; ++i)
	{
		pickupTextures[i].OnLostDevice();
	}
	uiTexture.OnLostDevice();

	SAFE_ON_LOST_DEVICE(gameOverFont);
	SAFE_ON_LOST_DEVICE(replayFont);

	Game::ReleaseAll();
}

//----------------------------------------------------------------------------------------------------

void GameplayState::ResetAll()
{
	for (int i = 0; i < 2; ++i)
	{
		backgroundTextures[i].OnResetDevice();
	}
	platformTexture.OnResetDevice();
	playerTexture.OnResetDevice();
	spinnerTexture.OnResetDevice();
	flyTexture.OnResetDevice();

	for (int i = 0; i < 2; ++i)
	{
		pickupTextures[i].OnResetDevice();
	}
	uiTexture.OnResetDevice();

	SAFE_ON_RESET_DEVICE(gameOverFont);
	SAFE_ON_RESET_DEVICE(replayFont);

	Game::ResetAll();
}