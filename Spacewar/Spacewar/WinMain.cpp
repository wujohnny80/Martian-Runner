#define _CRTDBG_MAP_ALLOC		// for detecting memory leaks
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>				// for detecting memory leaks
#include <crtdbg.h>				// for detecting memory leaks

#include "GameplayState.h"

// Function prototypes
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
bool CreateMainWindow(HWND &, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);

// Game pointer
GameplayState* game = nullptr;

//====================================================================================================
// Starting point for a Windows application
//====================================================================================================
int WINAPI WinMain( HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	// Check for memory leak if debug build
    #if defined(DEBUG) | defined(_DEBUG)
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    #endif

	MSG	 msg;
	HWND hwnd = NULL;
	game = new GameplayState();

	// Create the window
	if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
		return 1;

	try 
	{
		// Initialize game
		game->Initialize(hwnd);

		// Main message loop
		int done = 0;
		while (!done)
		{
			// PeekMessage is a non-blocking method for checking for Windows messages.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
			{
				// Look for quit message
				if (msg.message == WM_QUIT)
					done = 1;

				// Decode and pass messages on to WinProc
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				game->Run(hwnd); // run main game loop
			}
		} // end while
		SAFE_DELETE(game);
		return msg.wParam;
	} // end try
	catch(const GameError &err)
	{
		MessageBox(NULL, err.GetMessage(), "Error", MB_OK);
	}
	catch(...)
	{
		game->DeleteAll();
		DestroyWindow(hwnd);
		MessageBox(NULL, "Unknown error has occured in game.", "Error", MB_OK);
	}
	SAFE_DELETE(game);
	return 0;
}

//====================================================================================================
// window event callback function
//====================================================================================================
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (game->MessageHandler(hwnd, msg, wParam, lParam));
}

//====================================================================================================
// Create the window
// Returns: false on error
//====================================================================================================
bool CreateMainWindow(HWND &hwnd, HINSTANCE hInstance, int nCmdShow) 
{ 
	WNDCLASSEX wcx; 

	// Fill in the window class structure with parameters 
	// that describe the main window. 
	wcx.cbSize = sizeof(wcx);				// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;	// redraw if size changes 
	wcx.lpfnWndProc = WinProc;				// points to window procedure 
	wcx.cbClsExtra = 0;						// no extra class memory 
	wcx.cbWndExtra = 0;						// no extra window memory 
	wcx.hInstance = hInstance;				// handle to instance 
	wcx.hIcon = NULL; 
	wcx.hCursor = LoadCursor(NULL,IDC_ARROW);					// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// black background brush 
	wcx.lpszMenuName =  NULL;									// name of menu resource 
	wcx.lpszClassName = CLASS_NAME;								// name of window class 
	wcx.hIconSm = NULL;											// small class icon 

	// Register the window class. 
	// RegisterClassEx returns 0 on error.
	if (RegisterClassEx(&wcx) == 0) // if error
		return false;

	// Set up the screen in windowed for fullscreen mode
	DWORD style;
	if(FULLSCREEN)
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	else
		style = WS_OVERLAPPEDWINDOW;

	const int SCREEN_X_CENTER = (GetSystemMetrics(SM_CXSCREEN) - GAME_WIDTH) / 2;	// x position to center the window
	const int SCREEN_Y_CENTER = (GetSystemMetrics(SM_CYSCREEN) - GAME_HEIGHT) / 2;	// y position to center the window

	// Create window
	hwnd = CreateWindow(CLASS_NAME,				// name of the window class
						GAME_TITLE,				// title bar text
						style,					// window style
						SCREEN_X_CENTER,		// x center of screen
						SCREEN_Y_CENTER,		// y center of screen
						GAME_WIDTH,				// width of window
						GAME_HEIGHT,			// height of the window
						(HWND) NULL,			// no parent window
						(HMENU) NULL,			// no menu
						hInstance,				// handle to application instance
						(LPVOID) NULL);			// no window parameters

	// If there was an error creating the window
	if (!hwnd)
		return false;

	if(!FULLSCREEN)
	{
		// Adjust window size so client area is GAME_WIDTH x GAME_HEIGHT
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		MoveWindow(hwnd,
				   SCREEN_X_CENTER, // left
				   SCREEN_Y_CENTER, // top
				   GAME_WIDTH + (GAME_WIDTH-clientRect.right), // right
				   GAME_HEIGHT + (GAME_HEIGHT-clientRect.bottom), // bottom
				   true);
	}

	// Show the window
	ShowWindow(hwnd, nCmdShow);

	return true;
}