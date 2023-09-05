#ifndef ABSTRACT_ENGINE_H
#define ABSTRACT_ENGINE_H

#include <string>
#include <windows.h>

struct sKeyState
{
	bool bPressed;
	bool bReleased;
	bool bHeld;
};

class ConsoleEngine
{
public:
	ConsoleEngine();

	void EnableSound();
	int ConstructConsole(int width, int height, int fontw, int fonth);
	
protected:
	int Error(const wchar_t* msg);

protected:
	int _nScreenWidth;
	int _nScreenHeight;

	HANDLE _hOriginalConsole;
	HANDLE _hConsole;
	HANDLE _hConsoleIn;

	CHAR_INFO* _bufScreen;
	std::wstring _AppName;

	short _keyOldState[256] = { 0 };
	short _keyNewState[256] = { 0 };

	bool _mouseOldState[5] = { 0 };
	bool _mouseNewState[5] = { 0 };

	sKeyState _keys[256];
	sKeyState _mouse[5];

	int _mousePosX;
	int _mousePosY;

	bool _bConsoleInFocus = true;
	bool _bEnableSound = false;

	SMALL_RECT _rectWindow;
};

#endif