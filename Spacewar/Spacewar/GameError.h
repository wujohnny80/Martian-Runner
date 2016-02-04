// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// gameError.h v1.0
// Error class thrown by game engine.

#ifndef _GAMEERROR_H_
#define _GAMEERROR_H_
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <exception>

namespace GameErrorNS
{
	// Error codes
	// Negative numbers are fatal errors that may require the game to be shutdown.
	// Positive numbers are warnings that do not require the game to be shutdown.
	const int FATAL_ERROR = -1;
	const int WARNING = 1;
}

// Game Error class. Thrown when an error is detected by the game engine.
// Inherits from std::exception
class GameError : public std::exception
{
public:
	GameError() throw(): errorCode(GameErrorNS::FATAL_ERROR), message("Undefined Error in game.") {}
	GameError(const GameError& e) throw(): std::exception(e), errorCode(e.errorCode), message(e.message) {}
	GameError(int code, const std::string &s) throw(): errorCode(code), message(s) {}
	GameError& operator= (const GameError& rhs) throw() 
	{
		std::exception::operator=(rhs);
		this->errorCode = rhs.errorCode;
		this->message = rhs.message;
	}
	virtual ~GameError() throw() {}

	// override from base class
	virtual const char* what() const throw() { return this->GetMessage(); }

	const char* GetMessage() const throw() { return message.c_str(); }
	int GetErrorCode() const throw() { return errorCode; }
private:
	int errorCode;
	std::string message;
};
#endif // _GAMEERROR_H_