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

	_sAppName = L"Default";
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

sKeyState ConsoleEngine::GetKey(int nKeyID)
{
	return _keys[nKeyID];
}

int ConsoleEngine::GetMouseX()
{
	return _mousePosX;
}

int ConsoleEngine::GetMouseY()
{
	return _mousePosY;
}

sKeyState ConsoleEngine::GetMouse(int nMouseButtonID)
{
	return _mouse[nMouseButtonID];
}

bool ConsoleEngine::IsFocused()
{
	return _bConsoleInFocus;
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
	if (!OnUserCreate())
		_bAtomActive = false;

	/*if (_bEnableSound)
	{
		if (!CreateAudio())
		{
			_bAtomActive = false;
			_bEnableSound = false;
		}
	}*/

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	while (_bAtomActive)
	{
		while (_bAtomActive)
		{
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;

			tp1 = tp2;

			float fElapsedTime = elapsedTime.count();

			for (int i = 0; i < 256; ++i)
			{
				_keyNewState[i] = GetAsyncKeyState(i);

				_keys[i].bPressed = false;
				_keys[i].bReleased = false;

				if (_keyNewState[i] != _keyOldState[i])
				{
					if (_keyNewState[i] & 0x8000)
					{
						_keys[i].bPressed = !_keys[i].bHeld;
						_keys[i].bHeld = true;
					}
					else
					{
						_keys[i].bReleased = true;
						_keys[i].bHeld = false;
					}
				}
				_keyOldState[i] = _keyNewState[i];
			}

			INPUT_RECORD inBuf[32];
			DWORD events = 0;
			GetNumberOfConsoleInputEvents(_hConsoleIn, &events);

			if (events > 0)
				ReadConsoleInput(_hConsoleIn, inBuf, events, &events);

			for (DWORD i = 0; i < events; ++i)
			{
				switch (inBuf[i].EventType)
				{
				case FOCUS_EVENT:
				{
					_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
				}
				break;

				case MOUSE_EVENT:
				{
					switch (inBuf[i].Event.MouseEvent.dwEventFlags)
					{
					case MOUSE_MOVED:
					{
						_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
						_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
					}
					break;
					case 0:
					{
						for (int m = 0; m < 5; ++m)
							_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
					}
					break;

					default:
						break;
					}
				}
				break;

				default:
					break;
				}
			}

			for (int m = 0; m < 5; m++)
			{
				_mouse[m].bPressed = false;
				_mouse[m].bReleased = false;

				if (_mouseNewState[m] != _mouseOldState[m])
				{
					if (_mouseNewState[m])
					{
						_mouse[m].bPressed = true;
						_mouse[m].bHeld = true;
					}
					else
					{
						_mouse[m].bReleased = true;
						_mouse[m].bHeld = false;
					}
				}

				_mouseOldState[m] = _mouseNewState[m];
			}

			if (!OnUserUpdate(fElapsedTime))
				_bAtomActive = false;

			wchar_t s[256];
			swprintf_s(s, 256, L"SAMAN - Console Game Engine - %s - FPS: %3.2f", _sAppName.c_str(), 1.0f / fElapsedTime);
			SetConsoleTitle(s);
			WriteConsoleOutput(_hConsole, _bufScreen, { (short)_nScreenWidth, (short)_nScreenHeight }, { 0,0 }, &_rectWindow);
		}

		if (_bEnableSound)
		{

		}

		if (OnUserDestroy())
		{
			delete[] _bufScreen;
			SetConsoleActiveScreenBuffer(_hOriginalConsole);
			_cvGameFinished.notify_one();
		}
		else
		{
			_bAtomActive = true;
		}

	}
		

}

