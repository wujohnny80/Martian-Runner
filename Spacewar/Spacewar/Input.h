#ifndef _INPUT_H_
#define _INPUT_H_
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WindowsX.h>
#include <string>
#include <XInput.h>

#include "Constants.h"
#include "GameError.h"

// for high-definition mouse
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT) 0x02)
#endif

#pragma region Constants/Structs

namespace InputNS
{
	const int KEYS_ARRAY_LEN = 256; // size of key arrays

	// What values for clear(), bit flag
	const UCHAR KEYS_DOWN = 1;
	const UCHAR KEYS_PRESSED = 2;
	const UCHAR MOUSE = 4;
	const UCHAR TEXT_IN = 8;
	const UCHAR KEYS_MOUSE_TEXT = KEYS_DOWN + KEYS_PRESSED + MOUSE + TEXT_IN;
}

const short GAMEPAD_THUMBSTICK_DEADZONE = (short)(0.20f * 0X7FFF);	// default to 20% of range as deadzone
const short GAMEPAD_TRIGGER_DEADZONE = 20;							// trigger range 0-255
const DWORD MAX_CONTROLLERS = 4;									// Maximum number of controllers supported by XInput

// Bit corresponding to gamepad button in state.Gamepad.wButtons
const DWORD GAMEPAD_DPAD_UP        = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN      = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT      = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT     = 0x0008;
const DWORD GAMEPAD_START_BUTTON   = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON    = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB     = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB    = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER  = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A              = 0x1000;
const DWORD GAMEPAD_B              = 0x2000;
const DWORD GAMEPAD_X              = 0x4000;
const DWORD GAMEPAD_Y              = 0x8000;

struct ControllerState
{
	XINPUT_STATE        state;
	XINPUT_VIBRATION    vibration;
	float               vibrateTimeLeft;    // mSec
	float               vibrateTimeRight;   // mSec
	bool                connected;
};

#pragma endregion

class Input
{

public:
	Input();
	virtual ~Input();

	void Initialize(HWND hwnd, bool capture);

#pragma region Keys

	void KeyDown(WPARAM);	// save key down state
	void KeyUp(WPARAM);		// save key up state
	void KeyIn(WPARAM);		// save the char just entered in textIn string

	bool IsKeyDown(UCHAR vkey) const;		// returns true if the specified VIRTUAL KEY is down, otherwise false.
	bool WasKeyPressed(UCHAR vkey) const;	// returns true if the specified VIRTUAL KEY has been pressed in the most recent frame.
	bool AnyKeyPressed() const;				// returns true if any key was pressed in the most recent frame.

	void ClearKeyPress(UCHAR vkey);	// clear the specified key press
	void Clear(UCHAR what);			// clear specified input buffers
	void ClearAll() { Clear(InputNS::KEYS_MOUSE_TEXT); }
	void ClearTextIn() { textIn.clear(); }
	void ClearCharIn() { charIn = 0; }

	std::string GetTextIn() { return textIn; }
	char GetCharIn() { return charIn; }
	void SetTextIn(std::string str) { textIn = str; }

#pragma endregion

#pragma region Mouse

	void MouseIn(LPARAM);		// reads mouse screen position into mouseX, mouseY
	void MouseRawIn(LPARAM);	// reads raw mouse data into mouseRawX, mouseRawY
	void MouseWheelIn(WPARAM);	// Reads mouse wheel movement expressed in multiples of WHEEL_DELTA

	void SetLeftMouseButton(bool b)		{ mouseButtonL = b; }
	void SetMiddleMouseButton(bool b)	{ mouseButtonM = b; }
	void SetRightMouseButton(bool b)	{ mouseButtonR = b; }
	void SetXMouseButton(WPARAM wParam) 
	{
		mouseButtonX1 = (wParam & MK_XBUTTON1) ? true:false;
		mouseButtonX2 = (wParam & MK_XBUTTON2) ? true:false;
	}

	int GetMouseX() const { return mouseX; }
	int GetMouseY() const { return mouseY; }
	int GetMouseRawX()
	{ 
		// Return raw mouse X movement relative to previous position.
		// Left is <0, Right is >0
		// Compatible with high-definition mouse.
		int rawX = mouseRawX;
		mouseRawX = 0;
		return rawX; 
	}
	int GetMouseRawY()
	{
		// Return raw mouse Y movement relative to previous position.
		// Up is <0, Down is >0
		// Compatible with high-definition mouse.
		int rawY = mouseRawY;
		mouseRawY = 0;
		return rawY; 
	}
	int GetMouseWheel()
	{ 
		// Return mouse wheel movement relative to previous position.
		// Forward is >0, Backward is <0, movement is in multiples of WHEEL_DATA (120).
		int wheel = mouseWheel;
		mouseWheel = 0;
		return wheel; 
	}

	bool GetMouseLButton() const	{ return mouseButtonL; }
	bool GetMouseMButton() const	{ return mouseButtonM; }
	bool GetMouseRButton() const	{ return mouseButtonR; }
	bool GetMouseX1Button() const	{ return mouseButtonX1; }
	bool GetMouseX2Button() const	{ return mouseButtonX2; }

#pragma endregion

#pragma region Controller

	void CheckControllers();	// update connection status of game controllers    
	void ReadControllers();		// save input from connected game controllers

	void SetThumbstickDeadzone(short dz) { thumbstickDeadzone = abs(dz); }	// set thumbstick deadzone
	void SetTriggerDeadzone(BYTE dz) { triggerDeadzone = dz; }				// set trigger deadzone

	short GetThumbstickDeadzone() { return thumbstickDeadzone; }				// get thumbstick deadzone    
	BYTE getTriggerDeadzone() { return static_cast<BYTE>(triggerDeadzone); }	// get trigger deadzone

	const ControllerState* GetControllerState(UINT n) // return state of specified game controller
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return &controllers[n];
	}

	bool getGamepadConnected(UINT n)	// return connection state of specified game controller
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].connected;
	}

	const WORD getGamepadButtons(UINT n)	// return state of controller n buttons
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].state.Gamepad.wButtons;
	}

	bool GetGamepadDPadUp(UINT n)	// return state of controller n D-pad Up
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return ((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_UP) != 0);
	}

	bool GetGamepadDPadDown(UINT n)	// return state of controller n D-pad Down
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return ((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_DOWN) != 0);
	}

	bool GetGamepadDPadLeft(UINT n)	// return state of controller n D-pad Left
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return ((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_LEFT) != 0);
	}

	bool GetGamepadDPadRight(UINT n)	// return state of controller n D-pad Right
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_DPAD_RIGHT) != 0);
	}

	bool GetGamepadStart(UINT n)	// return state of controller n Start button.
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_START_BUTTON) != 0);
	}

	bool GetGamepadBack(UINT n)	// return state of controller n Back button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_BACK_BUTTON) != 0);
	}

	bool GetGamepadLeftThumb(UINT n)	// return state of controller n Left Thumb button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_LEFT_THUMB) != 0);
	}

	bool GetGamepadRightThumb(UINT n)	// return state of controller n Right Thumb button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_RIGHT_THUMB) != 0);
	}

	bool GetGamepadLeftShoulder(UINT n)	// return state of controller n Left Shoulder button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_LEFT_SHOULDER) != 0);
	}

	bool GetGamepadRightShoulder(UINT n)	// return state of controller n Right Shoulder button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_RIGHT_SHOULDER) != 0);
	}

	bool GetGamepadA(UINT n)	// return state of controller n A button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_A) != 0);
	}

	bool GetGamepadB(UINT n)	// return state of controller n B button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_B) != 0);
	}

	bool GetGamepadX(UINT n)	// return state of controller n X button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_X) != 0);
	}

	bool GetGamepadY(UINT n)	// return state of controller n Y button
	{
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return bool((controllers[n].state.Gamepad.wButtons&GAMEPAD_Y) != 0);
	}

	BYTE GetGamepadLeftTrigger(UINT n);
	BYTE GetGamepadLeftTriggerUndead(UINT n)	// return value of controller n Left Trigger (0 through 255).
	{
		// Deadzone is not applied
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].state.Gamepad.bLeftTrigger;
	}

	BYTE GetGamepadRightTrigger(UINT n);
	BYTE GetGamepadRightTriggerUndead(UINT n)	// return value of controller n Right Trigger (0 through 255)
	{
		// Deadzone is not applied
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].state.Gamepad.bRightTrigger;
	}

	SHORT GetGamepadThumbLX(UINT n);
	SHORT GetGamepadThumbLXUndead(UINT n)	// return value of controller n Left Thumbstick X (-32767 through 32767)
	{
		// Deadzone is not applied.
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].state.Gamepad.sThumbLX;
	}

	SHORT GetGamepadThumbLY(UINT n);
	SHORT GetGamepadThumbLYUndead(UINT n)	// return value of controller n Left Thumbstick Y (-32768 through 32767)
	{
		// Deadzone is not applied
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].state.Gamepad.sThumbLY;
	}

	SHORT GetGamepadThumbRX(UINT n);
	SHORT GetGamepadThumbRXUndead(UINT n)	// return value of controller n Right Thumbstick X (-32768 through 32767)
	{
		// Deadzone is not applied
		if(n > MAX_CONTROLLERS-1)   // if invalid controller number
			n=MAX_CONTROLLERS-1;    // force valid
		return controllers[n].state.Gamepad.sThumbRX;
	}

	SHORT GetGamepadThumbRY(UINT n);
	SHORT GetGamepadThumbRYUndead(UINT n)	// return value of controller n Right Thumbstick Y (-32768 through 32767)
	{
		// Deadzone is not applied
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		return controllers[n].state.Gamepad.sThumbRY;
	}

	void GamePadVibrateLeft(UINT n, WORD speed, float sec)	// vibrate controller n left motor
	{
		// Left is low frequency vibration
		// speed 0=off, 65536=100 percent
		// sec is time to vibrate in seconds
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		controllers[n].vibration.wLeftMotorSpeed = speed;
		controllers[n].vibrateTimeLeft = sec;
	}

	void GamePadVibrateRight(UINT n, WORD speed, float sec)	// vibrate controller n right motor
	{
		// Right is high frequency vibration
		// speed 0=off, 65536=100 percent
		// sec is time to vibrate in seconds
		if(n > MAX_CONTROLLERS-1)
			n=MAX_CONTROLLERS-1;
		controllers[n].vibration.wRightMotorSpeed = speed;
		controllers[n].vibrateTimeRight = sec;
	}

	void VibrateControllers(float frameTime);	// vibrates the connected controllers for the desired time

#pragma endregion

private:
	bool keysDown[InputNS::KEYS_ARRAY_LEN];			// true if specified key is down
	bool keysPressed[InputNS::KEYS_ARRAY_LEN];		// true if specified key was pressed
	bool newLine;									// true on start of new line
	std::string textIn;								// user entered text
	char charIn;									// last character entered
	int mouseX, mouseY;								// mouse screen coordinates
	int mouseRawX, mouseRawY;						// high-definition mouse data
	int mouseWheel;									// mouse wheel movement

	RAWINPUTDEVICE Rid[1];							// for high-definition mouse
	bool mouseCaptured;								// true if mouse captured
	bool mouseButtonL;								// true if left mouse button down
	bool mouseButtonM;								// true if middle mouse button down
	bool mouseButtonR;								// true if right mouse button down
	bool mouseButtonX1;								// true if X1 mouse button down
	bool mouseButtonX2;								// true if X2 mouse button down
	ControllerState controllers[MAX_CONTROLLERS];	// state of controllers
	short thumbstickDeadzone;
	short triggerDeadzone;
};
#endif // _INPUT_H_