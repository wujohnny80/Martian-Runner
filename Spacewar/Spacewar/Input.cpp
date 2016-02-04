#include "Input.h"

Input::Input()
	: newLine (true)			// start new line
	, textIn ("")				// clear textIn
	, charIn (0)				// clear charIn
	, mouseX (0)				// screen X
	, mouseY (0)				// screen Y
	, mouseRawX (0)				// high-definition X
	, mouseRawY (0)				// high-definition Y
	, mouseWheel (0)			// mouse wheel position
	, mouseButtonL (false)		// true if left mouse button is down
	, mouseButtonM (false)		// true if middle mouse button is do
	, mouseButtonR (false)		// true if right mouse button is dow
	, mouseButtonX1 (false)		// true if X1 mouse button is down
	, mouseButtonX2 (false)		// true if X2 mouse button is down
{
	for (size_t i = 0; i < InputNS::KEYS_ARRAY_LEN; i++)	// clear key down array
		keysDown[i] = false;	
	for (size_t i = 0; i < InputNS::KEYS_ARRAY_LEN; i++)	// clear key pressed array
		keysPressed[i] = false;

	for(int i=0; i<MAX_CONTROLLERS; i++)
	{
		controllers[i].vibrateTimeLeft = 0;
		controllers[i].vibrateTimeRight = 0;
	}
	thumbstickDeadzone = GAMEPAD_THUMBSTICK_DEADZONE;
	triggerDeadzone = GAMEPAD_TRIGGER_DEADZONE;
}

//----------------------------------------------------------------------------------------------------

Input::~Input()
{
	if(mouseCaptured)
		ReleaseCapture();
}

//----------------------------------------------------------------------------------------------------

void Input::Initialize(HWND hwnd, bool capture)
{
	// Initialize mouse and controller input.
	// Throws GameError
	// Capture = true to capture mouse.

	try
	{
		mouseCaptured = capture;

		// Register high-definition mouse
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC; 
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE; 
		Rid[0].dwFlags = RIDEV_INPUTSINK;   
		Rid[0].hwndTarget = hwnd;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		if(mouseCaptured)
			SetCapture(hwnd); // capture mouse

		ZeroMemory(controllers, sizeof(ControllerState) * MAX_CONTROLLERS);	// clear controllers state
		CheckControllers(); // check for connected controllers
	}
	catch(...)
	{
		throw(GameError(GameErrorNS::FATAL_ERROR, "Error initializing input system"));
	}
}

//----------------------------------------------------------------------------------------------------

void Input::KeyDown(WPARAM wParam)
{
	// Make sure key code is within buffer range
	if (wParam < InputNS::KEYS_ARRAY_LEN)
	{
		keysDown[wParam] = true; // update keysDown array
		// Key has been pressed, erased by clear()
		keysPressed[wParam] = true; // update keysPressed array
	}
}

//----------------------------------------------------------------------------------------------------

void Input::KeyUp(WPARAM wParam)
{
	// Make sure key code is within buffer range
	if (wParam < InputNS::KEYS_ARRAY_LEN)
		keysDown[wParam] = false;	// update state table
}

//----------------------------------------------------------------------------------------------------

void Input::KeyIn(WPARAM wParam)
{
	if (newLine) // if start of new line
	{
		textIn.clear();
		newLine = false;
	}

	if (wParam == '\b') // if backspace
	{
		if(textIn.length() > 0) // if characters exist
			textIn.erase(textIn.size()-1); // erase last character entered
	}
	else
	{
		textIn += wParam; // add character to textIn
		charIn = wParam; // save last char entered
	}

	if ((char)wParam == '\r') // if return
		newLine = true; // start new line
}

//----------------------------------------------------------------------------------------------------

bool Input::IsKeyDown(UCHAR vkey) const
{
	if (vkey < InputNS::KEYS_ARRAY_LEN)
		return keysDown[vkey];
	else
		return false;
}

//----------------------------------------------------------------------------------------------------

bool Input::WasKeyPressed(UCHAR vkey) const
{
	// Key presses are erased at the end of each frame.
	if (vkey < InputNS::KEYS_ARRAY_LEN)
		return keysPressed[vkey];
	else
		return false;
}

//----------------------------------------------------------------------------------------------------

bool Input::AnyKeyPressed() const
{
	// Key presses are erased at the end of each frame.
	for (size_t i = 0; i < InputNS::KEYS_ARRAY_LEN; i++)
		if(keysPressed[i] == true)
			return true;
	return false;
}

//----------------------------------------------------------------------------------------------------

void Input::ClearKeyPress(UCHAR vkey)
{
	if (vkey < InputNS::KEYS_ARRAY_LEN)
		keysPressed[vkey] = false;
}

//----------------------------------------------------------------------------------------------------

void Input::Clear(UCHAR what)
{
	// Clear specified input buffers where what is any combination of
	// KEYS_DOWN, KEYS_PRESSED, MOUSE, TEXT_IN or KEYS_MOUSE_TEXT.
	// Use OR '|' operator to combine parameters.
	if(what & InputNS::KEYS_DOWN) // if clear keys down
	{
		for (size_t i = 0; i < InputNS::KEYS_ARRAY_LEN; i++)
			keysDown[i] = false;
	}
	if(what & InputNS::KEYS_PRESSED) // if clear keys pressed
	{
		for (size_t i = 0; i < InputNS::KEYS_ARRAY_LEN; i++)
			keysPressed[i] = false;
	}
	if(what & InputNS::MOUSE) // if clear mouse
	{
		mouseX = 0;
		mouseY = 0;
		mouseRawX = 0;
		mouseRawY = 0;
		mouseWheel = 0;
	}
	if(what & InputNS::TEXT_IN)
	{
		ClearTextIn();
		ClearCharIn();
	}
}

//----------------------------------------------------------------------------------------------------

void Input::MouseIn(LPARAM lParam)
{
	mouseX = GET_X_LPARAM(lParam); 
	mouseY = GET_Y_LPARAM(lParam);
}

//----------------------------------------------------------------------------------------------------

void Input::MouseRawIn(LPARAM lParam)
{
	// This routine is compatible with a high-definition mouse
	UINT dwSize = 40;
	static BYTE lpb[40];

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, 
		lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE) 
	{
		mouseRawX = raw->data.mouse.lLastX;
		mouseRawY = raw->data.mouse.lLastY;
	} 
}

//----------------------------------------------------------------------------------------------------

void Input::MouseWheelIn(WPARAM wParam)
{
	// Reads mouse wheel movement expressed in multiples of WHEEL_DELTA, which
	// is 120. A positive value indicates that the wheel was rotated away from the
	// user, a negative value indicates that the wheel was rotated toward the user.
	mouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
}

//----------------------------------------------------------------------------------------------------

void Input::CheckControllers()
{
	DWORD result;
	for( DWORD i = 0; i <MAX_CONTROLLERS; i++)
	{
		result = XInputGetState(i, &controllers[i].state);
		if(result == ERROR_SUCCESS)
			controllers[i].connected = true;
		else
			controllers[i].connected = false;
	}
}

//----------------------------------------------------------------------------------------------------

void Input::ReadControllers()
{
	DWORD result;
	for( DWORD i = 0; i <MAX_CONTROLLERS; i++)
	{
		if(controllers[i].connected)
		{
			result = XInputGetState(i, &controllers[i].state);
			if(result == ERROR_DEVICE_NOT_CONNECTED) // if controller disconnected
				controllers[i].connected = false;
		}
	}
}

//----------------------------------------------------------------------------------------------------

BYTE Input::GetGamepadLeftTrigger(UINT n) 
{
	// Return value of controller n Left Trigger (0 through 255)
	// Trigger movement less than GAMEPAD_TRIGGER_DEADZONE returns 0
	// Return value is scaled to 0 through 255
	BYTE value = GetGamepadLeftTriggerUndead(n);
	if(value > triggerDeadzone)
		// Adjust magnitude relative to the end of the dead zone
		value = (value - triggerDeadzone) * 255 / (255-triggerDeadzone);
	else
		value = 0;
	return value;
}

//----------------------------------------------------------------------------------------------------

BYTE Input::GetGamepadRightTrigger(UINT n) 
{
	// Return value of controller n Right Trigger (0 through 255)
	// Trigger movement less than GAMEPAD_TRIGGER_DEADZONE returns 0
	// Return value is scaled to 0 through 255
	BYTE value = GetGamepadRightTriggerUndead(n);
	if(value > triggerDeadzone)
		// Adjust magnitude relative to the end of the dead zone
		value = (value - triggerDeadzone) * 255 / (255-triggerDeadzone);
	else
		value = 0;
	return value;
}

//----------------------------------------------------------------------------------------------------

SHORT Input::GetGamepadThumbLX(UINT n)
{
	// Return value of controller n Left Thumbstick X (-32767 through 32767)
	// Stick movement less than GAMEPAD_THUMBSTICK_DEADZONE returns 0
	// Return value is scaled to -32768 through 32767
	int x = GetGamepadThumbLXUndead(n);
	if(x > thumbstickDeadzone) // if +x outside dead zone
		// Adjust x relative to the deadzone and scale to 0 through 32767
		x = (x - thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else if(x < -thumbstickDeadzone) // if -x outside dead zone
		// Adjust x relative to the deadzone and scale to 0 through -32767
		x = (x + thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else // x inside dead zone
		x = 0;
	return static_cast<SHORT>(x);
}

//----------------------------------------------------------------------------------------------------

SHORT Input::GetGamepadThumbLY(UINT n)
{
	// Return value of controller n Left Thumbstick Y (-32768 through 32767).
	// Stick movement less than GAMEPAD_THUMBSTICK_DEADZONE returns 0.
	// Return value is scaled to -32768 through 32767
	int y = GetGamepadThumbLYUndead(n);
	if(y > thumbstickDeadzone) // if +y outside dead zone
		// Adjust y relative to the deadzone and scale to 0 through 32767
		y = (y - thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else if(y < -thumbstickDeadzone) // if -y outside dead zone
		// Adjust y relative to the deadzone and scale to 0 through -32767
		y = (y + thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else // y inside dead zone
		y = 0;
	return static_cast<SHORT>(y);
}

//----------------------------------------------------------------------------------------------------

SHORT Input::GetGamepadThumbRX(UINT n)
{
	// Return value of controller n Right Thumbstick X (-32768 through 32767)
	// Stick movement less than GAMEPAD_THUMBSTICK_DEADZONE returns 0
	// Return value is scaled to -32768 through 32767
	int x = GetGamepadThumbRXUndead(n);
	if(x > thumbstickDeadzone) // if +x outside dead zone
		// Adjust x relative to the deadzone and scale to 0 through 32767
		x = (x - thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else if(x < -thumbstickDeadzone) // if -x outside dead zone
		// Adjust x relative to the deadzone and scale to 0 through -32767
		x = (x + thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else // x inside dead zone
		x = 0;
	return static_cast<SHORT>(x);
}

//----------------------------------------------------------------------------------------------------

SHORT Input::GetGamepadThumbRY(UINT n) 
{
	// Return value of controller n Right Thumbstick Y (-32768 through 32767).
	// Stick movement less than GAMEPAD_THUMBSTICK_DEADZONE returns 0.
	// Return value is scaled to -32768 through 32767
	int y = GetGamepadThumbRYUndead(n);
	if(y > thumbstickDeadzone) // if +y outside dead zone
		// Adjust y relative to the deadzone and scale to 0 through 32767
		y = (y - thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else if(y < -thumbstickDeadzone) // if -y outside dead zone
		// Adjust y relative to the deadzone and scale to 0 through -32767
		y = (y + thumbstickDeadzone) * 32767 / (32767-thumbstickDeadzone);
	else // y inside dead zone
		y = 0;
	return static_cast<SHORT>(y);
}

//----------------------------------------------------------------------------------------------------

void Input::VibrateControllers(float frameTime)
{
	for(int i=0; i < MAX_CONTROLLERS; i++)
	{
		if(controllers[i].connected)
		{
			controllers[i].vibrateTimeLeft -= frameTime;
			if(controllers[i].vibrateTimeLeft < 0)
			{
				controllers[i].vibrateTimeLeft = 0;
				controllers[i].vibration.wLeftMotorSpeed = 0;
			}
			controllers[i].vibrateTimeRight -= frameTime;
			if(controllers[i].vibrateTimeRight < 0)
			{
				controllers[i].vibrateTimeRight = 0;
				controllers[i].vibration.wRightMotorSpeed = 0;
			}
			XInputSetState(i, &controllers[i].vibration);
		}
	}
}

