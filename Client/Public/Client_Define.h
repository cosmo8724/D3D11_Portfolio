#pragma once
#include <process.h>

namespace Client
{
	static unsigned int g_iWinSizeX = 1280;
	static unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_PUBLIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TESTSTAGE, LEVEL_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;

using namespace Client;