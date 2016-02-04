// Modified-by: Johnny Wu

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>

//----------------------------------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------------------------------

// font images
const char FONT_IMAGE[] = "./Assets/Font/CKfont.png";

// audio files
const char WAVE_BANK[] = "";
const char SOUND_BANK[] = "";

// Window
const char CLASS_NAME[] = "WinMain";
const char GAME_TITLE[] = "Final Project";
const bool FULLSCREEN = false;

const UINT  GAME_WIDTH = 1440;
const UINT  GAME_HEIGHT = 900;

// Game
const double PI = 3.14159265;
const float FRAME_RATE  = 200.0f;					// the target frame rate (frames/sec)
const float MIN_FRAME_RATE = 10.0f;					// the minimum frame rate
const float MIN_FRAME_TIME = 1.0f/FRAME_RATE;		// minimum desired time for 1 frame
const float MAX_FRAME_TIME = 1.0f/MIN_FRAME_RATE;	// maximum time used in calculations

// Key mappings
// In this game simple constants are used for key mappings. If variables were used
// it would be possible to save and restore key mappings from a data file.
namespace Key
{
	const UCHAR TILDE =				VK_OEM_3;		// `~ key for U.S.
	const UCHAR ESC =				VK_ESCAPE;		// Escape
	const UCHAR ENTER =				VK_RETURN;		// Enter key
	const UCHAR SPACE =				VK_SPACE;		// Space key
	const UCHAR ALT =				VK_MENU;		// Alt key
	const UCHAR PGUP =				VK_PRIOR;		// Page up
	const UCHAR PGDN =				VK_NEXT;		// Page down
	const UCHAR LEFT_ARROW =		VK_LEFT;		// left arrow
	const UCHAR RIGHT_ARROW =		VK_RIGHT;		// right arrow
	const UCHAR UP_ARROW =			VK_UP;			// up arrow
	const UCHAR DOWN_ARROW =		VK_DOWN;		// down arrow
	const UCHAR L_MOUSE_BUTTON =    VK_LBUTTON;		// Left mouse button
	const UCHAR R_MOUSE_BUTTON =    VK_RBUTTON;		// Right mouse button

	const UCHAR ZERO =		0x30;
	const UCHAR ONE =		0x31;
	const UCHAR TWO =		0x32;
	const UCHAR THREE =		0x33;
	const UCHAR FOUR =		0x34;
	const UCHAR FIVE =		0x35;
	const UCHAR SIX =		0x36;
	const UCHAR SEVEN =		0x37;
	const UCHAR EIGHT =		0x38;
	const UCHAR NINE =		0x39;
	const UCHAR A =			0x41;
	const UCHAR B =			0x42;
	const UCHAR C =			0x43;
	const UCHAR D =			0x44;
	const UCHAR E =			0x45;
	const UCHAR F =			0x46;
	const UCHAR G =			0x47;
	const UCHAR H =			0x48;
	const UCHAR I =			0x49;
	const UCHAR J =			0x4A;
	const UCHAR K =			0x4B;
	const UCHAR L =			0x4C;
	const UCHAR M =			0x4D;
	const UCHAR N =			0x4E;
	const UCHAR O =			0x4F;
	const UCHAR P =			0x50;
	const UCHAR Q =			0x51;
	const UCHAR R =			0x52;
	const UCHAR S =			0x53;
	const UCHAR T =			0x54;
	const UCHAR U =			0x55;
	const UCHAR V =			0x56;
	const UCHAR W =			0x57;
	const UCHAR X =			0x58;
	const UCHAR Y =			0x59;
	const UCHAR Z =			0x5A;
}

// Defines
#define TRANSCOLOR SETCOLOR_ARGB(0,255,0,255) // transparent color (magenta)
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define RAND_MAX 0x7fff

// Safely release pointer referenced item
template <typename T>
inline void SafeRelease(T& ptr)
{
	if (ptr)
	{ 
		ptr->Release(); 
		ptr = NULL;
	}
}
#define SAFE_RELEASE SafeRelease            // for backward compatiblility

// Utility functions
inline float Clamp(float value, float min, float max)
{
	return value < min ? min : (value > max ? max : value);
}

inline float RandomFloat(float fLower, float fUpper)
{
	// Create a random float from 0.0f to 1.0f
	float fRandFloat = rand() / static_cast<float>(RAND_MAX);

	// Return a number between fLower and fUpper
	return fLower + ((fUpper - fLower) * fRandFloat);
}

inline int RandomInt(int iLower, int iUpper)
{
	// Create a random float from 0.0f to 1.0f
	float fRandFloat = rand() / static_cast<float>(RAND_MAX);

	// Return a number between iLower and iUpper
	return iLower + static_cast<int>((iUpper - iLower) * fRandFloat);
}

// Safely delete pointer referenced item
template <typename T>
inline void SafeDelete(T& ptr)
{
	if (ptr)
	{ 
		delete ptr; 
		ptr = NULL;
	}
}
#define SAFE_DELETE SafeDelete // for backward compatiblility

// Safely delete pointer referenced array
template <typename T>
inline void SafeDeleteArray(T& ptr)
{
	if (ptr)
	{ 
		delete[] ptr; 
		ptr = NULL;
	}
}
#define SAFE_DELETE_ARRAY SafeDeleteArray   // for backward compatiblility

// Safely call onLostDevice
template <typename T>
inline void SafeOnLostDevice(T& ptr)
{
	if (ptr)
		ptr->onLostDevice(); 
}
#define SAFE_ON_LOST_DEVICE SafeOnLostDevice // for backward compatiblility

// Safely call onResetDevice
template <typename T>
inline void SafeOnResetDevice(T& ptr)
{
	if (ptr)
		ptr->onResetDevice(); 
}
#define SAFE_ON_RESET_DEVICE SafeOnResetDevice  // for backward compatiblility

#endif // _CONSTANTS_H_
