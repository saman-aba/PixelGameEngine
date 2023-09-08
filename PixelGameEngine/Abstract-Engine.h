#ifndef ABSTRACT_ENGINE_H
#define ABSTRACT_ENGINE_H

#include <string>
#include <windows.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>

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

	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate() = 0;
	virtual bool OnUserDestroy() = 0;

	void EnableSound();
	int ConstructConsole(int width, int height, int fontw, int fonth);
	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F);
	void Fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);

	void Clip(int& x, int& y);

	void Start();

	int ScreenWidth();
	int ScreenHeight();
	
	sKeyState GetKey(int nKeyID);
	int GetMouseX();
	int GetMouseY();
	sKeyState GetMouse(int nMouseButtonID);
	bool IsFocused();
		
protected:
	int Error(const wchar_t* msg);

	static BOOL CloseHandler(DWORD evt);

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

	static std::atomic<bool> _bAtomActive;
	static std::condition_variable _cvGameFinished;
	static std::mutex _muxGame;

private:
	void GameThread();
};

#endif