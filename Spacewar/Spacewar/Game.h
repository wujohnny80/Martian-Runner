#ifndef _GAME_H_
#define _GAME_H_
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MMSystem.h>

#include "Console.h"
#include "Constants.h"
#include "GameError.h"
#include "Graphics.h"
#include "Input.h"
#include "TextDX.h"

namespace GameNS
{
	const char FONT[] = "Courier New";
	const int POINT_SIZE = 14;
	const COLOR_ARGB FONT_COLOR = SETCOLOR_ARGB(255, 255, 255, 255);
}

class Game
{
public:
	Game();
	virtual ~Game();

#pragma region Virtual Functions

	// Initialize the game
	virtual void Initialize(HWND hwnd);

	// Call run repeatedly by the main message loop in WinMain
	virtual void Run(HWND hwnd);

	// Render game items
	virtual void RenderGame();

	// Call when the graphics device was lost
	// Release all reserved video memory so graphics device may be reset
	virtual void ReleaseAll();

	// Recreate all surfaces and reset all entities
	virtual void ResetAll();

	// Delete all reserved memory
	virtual void DeleteAll();

	// Handle lost graphics device
	virtual void HandleLostGraphicsDevice();

	// Process console commands
	virtual void ConsoleCommand();

	virtual void Restart() {}

#pragma endregion

#pragma region Pure Virtuals
	// Update game items.
	virtual void Update() = 0;

	// Perform AI calculations
	virtual void AI() = 0;

	// Check for collisions
	virtual void Collisions() = 0;

	// Render graphics
	// Call graphics->spriteBegin();
	// draw sprites
	// Call graphics->spriteEnd();
	// draw non-sprites
	virtual void Render() = 0;
#pragma endregion

#pragma region Member Functions
	// Window message handler
	LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Set display mode (fullscreen, window or toggle)
	void SetDisplayMode(GraphicsNS::DISPLAY_MODE mode = GraphicsNS::TOGGLE);

	void ExitGame() { PostMessage(hwnd, WM_DESTROY, 0, 0); }
#pragma endregion

#pragma region Accessors/Mutators
	Graphics* GetGraphics() { return graphics; }
	Input* getInput() { return input; }
#pragma endregion

protected:
	Console*		console;
	Graphics*		graphics;
	Input*			input;
	TextDX			DXFont;
	HWND			hwnd;			// window handle
	HRESULT			hr;				// standard return type
	LARGE_INTEGER	timeStart;		// performance counter start value
	LARGE_INTEGER	timeEnd;		// performance counter end value
	LARGE_INTEGER	timerFreq;		// performance counter frequency
	float			frameTime;		// time required for last frame
	float			fps;			// frames per second
	DWORD			sleepTime;		// milliseconds to sleep between frames
	bool			fpsOn;
	bool			paused;			// true if game is paused
	bool			initialized;
	std::string		command;
};
#endif // _GAME_H_