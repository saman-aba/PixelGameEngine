#include "Abstract-Engine.h"

ConsoleEngine::ConsoleEngine()
{
	_nScreenHeight = 80;
	_nScreenWidth = 30;

	_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	std::memset(_keyNewState, 0, 256 * sizeof(short));
	std::memset(_keyOldState, 0, 256 * sizeof(short));

	std::memset(_keys, 0, 256 * sizeof(sKeyState));

	_mousePosX = 0;
	_mousePosY = 0;

	_bEnableSound = false;

	_AppName = L"Default";
}

void ConsoleEngine::EnableSound()
{
	_bEnableSound = true;
}

int ConsoleEngine::ConstructConsole(int width, int height, int fontw, int fonth)
{
	if (_hConsole == INVALID_HANDLE_VALUE)
		return Error(L"Bad Handle");

	_nScreenWidth = width;
	_nScreenHeight = height;

	_rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(_hConsole, TRUE, &_rectWindow);

	COORD coord = { (short)_nScreenWidth, (short)_nScreenHeight };
	if(!SetConsoleScreenBufferSize(_hConsole, coord))
		Error(L"SetConsoleScreenBufferSize");

	if (!SetConsoleActiveScreenBuffer(_hConsole))
	return 0;
}

int ConsoleEngine::Error(const wchar_t* msg)
{
	wchar_t buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(_hOriginalConsole);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
	return 0;
}

