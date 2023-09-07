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
		return Error(L"SetConsoleActiveScreenBuffer");

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontw;
	cfi.dwFontSize.Y = fonth;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(_hConsole, false, &cfi))
		return Error(L"SetCurrentConsoleFontEx");

	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(_hConsole, &csbi))
		return Error(L"SetConsoleScreenBufferInfo");

	if (_nScreenHeight > csbi.dwMaximumWindowSize.Y)
		return Error(L"Screen Height / Font Height Too Big");

	if (_nScreenWidth > csbi.dwMaximumWindowSize.X)
		return Error(L"Screen Width / Font Width Too Big");

	short shortScreenWidth = (short)_nScreenWidth - 1;
	short shortScreenHeight = (short)_nScreenHeight - 1;
	_rectWindow = { 0, 0, shortScreenWidth , shortScreenHeight };
	
	if (!SetConsoleWindowInfo(_hConsole, TRUE, &_rectWindow))
		return Error(L"SetConsoleWindowInfo");

	if (!SetConsoleMode(_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return Error(L"SetConsoleMode");

	_bufScreen = new CHAR_INFO[_nScreenHeight * _nScreenWidth];
	memset(_bufScreen, 0, sizeof(CHAR_INFO) * _nScreenWidth * _nScreenHeight);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
	return 1;

}

void ConsoleEngine::Draw(int x, int y, short c , short col)
{
	if (x >= 0 && x < _nScreenWidth && y >= 0 && y < _nScreenHeight)
	{
		_bufScreen[y * _nScreenWidth + x].Char.UnicodeChar = c;
		_bufScreen[y * _nScreenWidth + x].Attributes = col;
	}
}

void ConsoleEngine::Fill(int x1, int y1, int x2, int y2, short c, short col)
{
	Clip(x1, y1);
	Clip(x2, y2);
	for (int x = x1; x < x2; ++x)
		for (int y = y1; y < y2; ++y)
			Draw(x, y, c, col);
}

void ConsoleEngine::Clip(int& x, int& y)
{
	if (x < 0) x = 0;
	if (x > _nScreenWidth) x = _nScreenWidth;
	if (y < 0) y = 0;
	if (y > _nScreenHeight) y = _nScreenHeight;
}

void ConsoleEngine::Start()
{
	_bAtomActive = true;

	std::thread t = std::thread(&ConsoleEngine::GameThread, this);

	t.join();
}

int ConsoleEngine::ScreenWidth()
{
	return _nScreenWidth;
}

int ConsoleEngine::ScreenHeight()
{
	return _nScreenHeight;
}


int ConsoleEngine::Error(const wchar_t* msg)
{
	wchar_t buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(_hOriginalConsole);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
	return 0;
}

BOOL ConsoleEngine::CloseHandler(DWORD evt)
{
	if (evt == CTRL_CLOSE_EVENT)
	{
		_bAtomActive = false;

		std::unique_lock<std::mutex> ul(_muxGame);
		_cvGameFinished.wait(ul);
	}
	return true;
}

void ConsoleEngine::GameThread()
{

}

