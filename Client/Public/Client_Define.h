#pragma once
#include <process.h>

#ifdef _DEBUG
#define new DBG_NEW 
#endif

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_TESTSTAGE, LEVEL_END };
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;

using namespace Client;