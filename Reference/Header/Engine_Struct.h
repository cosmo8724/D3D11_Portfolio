#pragma once

namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE		eWindowMode;
		unsigned int	iViewportSizeX;
		unsigned int	iViewportSizeY;
		HINSTANCE	hInst;
		HWND			hWnd;
	} GRAPHIC_DESC;
}