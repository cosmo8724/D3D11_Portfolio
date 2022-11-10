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

	typedef struct tagVertexPositionTexture
	{
		XMFLOAT3	vPosition;
		XMFLOAT2	vTexUV;
	} VTXTEX;

	typedef struct tagFaceIndices16
	{
		unsigned short	_0, _1, _2;
	} FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_0, _1, _2;
	} FACEINDICES32;
}