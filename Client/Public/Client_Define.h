#pragma once
#include <process.h>
#include <ctime>

#ifdef _DEBUG
#define new DBG_NEW 
#endif


namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_TESTSTAGE, LEVEL_END };
	class CCursor;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;
extern Client::CCursor*		g_pCursor;
extern _bool			g_bReadySceneChange;
extern _bool			g_bShopOpen;

using namespace Client;