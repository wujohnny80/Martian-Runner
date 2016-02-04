#include "Game.h"

// The primary class should inherit from Game class
Game::Game()
	: console(nullptr)
	, graphics (nullptr)
	, paused (false)
	, initialized(false)
	, fps(100)
	, fpsOn(false)
{
	input = new Input(); // initialize keyboard input immediately
	// additional initialization is handled in later call to input->Initialize()
}

//----------------------------------------------------------------------------------------------------

Game::~Game()
{
	DeleteAll(); // free all reserved memory
	ShowCursor(true);
}

//----------------------------------------------------------------------------------------------------

void Game::Initialize(HWND hw)
{
	hwnd = hw;
	graphics = new Graphics();
	graphics->Initialize(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN); // throws GameError

	// Initialize input, do not capture mouse
	input->Initialize(hwnd, false); // throws GameError

	// Initialize console
	console = new Console();
	console->initialize(graphics, input);
	console->print("/help for list of commands");

	// Initialize DXFont
	if (!DXFont.initialize(graphics, GameNS::POINT_SIZE, false, false, GameNS::FONT))
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing DirectX font"));
	DXFont.setFontColor(GameNS::FONT_COLOR);

	// Attempt to set up high resolution timer
	if(QueryPerformanceFrequency(&timerFreq) == false)
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing high resolution timer"));

	QueryPerformanceCounter(&timeStart); // get starting time

	initialized = true;
}

//----------------------------------------------------------------------------------------------------

void Game::Run(HWND hwnd)
{
	if(graphics == NULL) // if graphics not initialized
		return;

	// Calculate elapsed time of last frame, save in frameTime
	QueryPerformanceCounter(&timeEnd);
	frameTime = (float)(timeEnd.QuadPart - timeStart.QuadPart ) / (float)timerFreq.QuadPart;

	// Power saving code, requires winmm.lib
	// if not enough time has elapsed for desired frame rate
	if (frameTime < MIN_FRAME_TIME)
	{
		sleepTime = (DWORD)((MIN_FRAME_TIME - frameTime)*1000);
		timeBeginPeriod(1);			// request 1mS resolution for windows timer
		Sleep(sleepTime);			// release cpu for sleepTime
		timeEndPeriod(1);			// end 1mS timer resolution
		return;
	}

	if (frameTime > 0.0)
		fps = (fps*0.99f) + (0.01f/frameTime); // average fps
	if (frameTime > MAX_FRAME_TIME) // if frame rate is very slow
		frameTime = MAX_FRAME_TIME; // limit maximum frameTime
	timeStart = timeEnd;

	// Update(), AI(), and Collisions() are pure virtual functions.
	// These functions must be provided in the class that inherits from Game.
	if (!paused)
	{
		Collisions(); // handle collisions
		Update(); // update all game items
		AI(); // artificial intelligence
		//input->VibrateControllers(frameTime); // handle controller vibration
	}
	RenderGame();

	//check for console key
	if (input->WasKeyPressed(Key::TILDE))
	{
		console->showHide();
		paused = console->getVisible(); // pause game when console is visible
	}
	ConsoleCommand();               // process user entered console command

	input->ReadControllers(); // read state of controllers

	// if Alt+Enter toggle fullscreen/window
	if (input->IsKeyDown(Key::ALT) && input->WasKeyPressed(Key::ENTER))
		SetDisplayMode(GraphicsNS::TOGGLE); // toggle fullscreen/window

	// if Esc key, set window mode
	//if (input->IsKeyDown(Key::ESC))
		//SetDisplayMode(GraphicsNS::WINDOW); // set window mode

	// Clear input
	// Call this after all key checks are done
	input->Clear(InputNS::KEYS_PRESSED);
}

//----------------------------------------------------------------------------------------------------

void Game::RenderGame()
{
	const int bufferSize = 20;
	static char buffer[bufferSize];

	if (SUCCEEDED(graphics->BeginScene()))
	{
		Render();
		graphics->SpriteBegin();
		if(fpsOn)
		{
			_snprintf(buffer, bufferSize, "Fps %d", (int)fps);
			DXFont.print(buffer, GAME_WIDTH - 100, GAME_HEIGHT - 28);
		}
		graphics->SpriteEnd();
		console->draw();
		graphics->EndScene();
	}
	HandleLostGraphicsDevice();

	// Display the back buffer on the screen
	graphics->ShowBackBuffer();
}

//----------------------------------------------------------------------------------------------------

void Game::ReleaseAll()
{
	SAFE_ON_LOST_DEVICE(console);
	DXFont.onLostDevice();
}

//----------------------------------------------------------------------------------------------------

void Game::ResetAll()
{
	DXFont.onResetDevice();
	SAFE_ON_RESET_DEVICE(console);
}

//----------------------------------------------------------------------------------------------------

void Game::DeleteAll()
{
	ReleaseAll(); // call OnLostDevice() for every graphics item
	SafeDelete(graphics);
	SafeDelete(input);
	initialized = false;
}

//----------------------------------------------------------------------------------------------------

LRESULT Game::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(initialized) // do not process messages if not initialized
	{
		switch(msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN: case WM_SYSKEYDOWN:	// key down
			input->KeyDown(wParam);
			return 0;
		case WM_KEYUP: case WM_SYSKEYUP:		// key up
			input->KeyUp(wParam);
			return 0;
		case WM_CHAR:							// character entered
			input->KeyIn(wParam);
			return 0;
		case WM_MOUSEMOVE:						// mouse moved
			input->MouseIn(lParam);
			return 0;
		case WM_INPUT:							// raw mouse data in
			input->MouseRawIn(lParam);
			return 0;
		case WM_LBUTTONDOWN:					// left mouse button down
			input->SetLeftMouseButton(true);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_LBUTTONUP:						// left mouse button up
			input->SetLeftMouseButton(false);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_MBUTTONDOWN:					// middle mouse button down
			input->SetMiddleMouseButton(true);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_MBUTTONUP:						// middle mouse button up
			input->SetMiddleMouseButton(false);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_RBUTTONDOWN:					// right mouse button down
			input->SetRightMouseButton(true);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_RBUTTONUP:						// right mouse button up
			input->SetRightMouseButton(false);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_XBUTTONDOWN: case WM_XBUTTONUP:	// mouse X button down/up
			input->SetXMouseButton(wParam);
			input->MouseIn(lParam);				// mouse position
			return 0;
		case WM_MOUSEWHEEL:						// mouse wheel move
			input->MouseWheelIn(wParam);
			return 0;
		//case WM_DEVICECHANGE:					// check for controller insert
		//	input->checkControllers();
		//	return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam); // let Windows handle it
}

//----------------------------------------------------------------------------------------------------

void Game::SetDisplayMode(GraphicsNS::DISPLAY_MODE mode)
{
	ReleaseAll(); // free all user created surfaces
	graphics->ChangeDisplayMode(mode);
	ResetAll(); // recreate surfaces
}

//----------------------------------------------------------------------------------------------------

void Game::HandleLostGraphicsDevice()
{
	// Test for and handle lost device
	hr = graphics->GetDeviceState();
	if(FAILED(hr)) // if graphics device is not in a valid state
	{
		// If the device is lost and not available for reset
		if(hr == D3DERR_DEVICELOST)
		{
			Sleep(100); // yield cpu time (100 mili-seconds)
			return;
		} 
		// The device was lost but is now available for reset
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			ReleaseAll();
			hr = graphics->Reset(); // attempt to reset graphics device
			if(FAILED(hr)) // if reset failed
				return;
			ResetAll();
		}
		else
			return; // other device error
	}
}

//----------------------------------------------------------------------------------------------------

void Game::ConsoleCommand()
{
	// Process console commands
	// Override this function in the derived class if new console commands are added.
	command = console->getCommand();    // get command from console
	if(command == "")                   // if no command
		return;

	if (command == "/help")              // if "help" command
	{
		console->print("Console Commands:");
		console->print("/fps - toggle display of frames per second");
		console->print("/quit - quit game");
		console->print("/restart - restart game");
		return;
	}

	if (command == "/fps")
	{
		fpsOn = !fpsOn;                 // toggle display of fps
		if(fpsOn)
			console->print("fps On");
		else
			console->print("fps Off");
    }

	if (command == "/quit")
	{
		ExitGame();
	}

	if (command == "/restart")
	{
		console->hide();
		paused = false;
		Restart();
	}
}