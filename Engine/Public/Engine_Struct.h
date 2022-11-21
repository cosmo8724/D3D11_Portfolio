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

	typedef struct tagVertexPositionNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	} VTXNORTEX;

	typedef struct tagLightDesc
	{
		enum LIGHTTYPE { LIGHT_DIRECTIONAL, LIGHT_POINT, LIGHTTYPE_END };

		LIGHTTYPE		eType;
		_bool				bIsLightOn;

		/* For Directional Light */
		XMFLOAT4		vDirection;

		/* For Point Light */
		XMFLOAT4		vPosition;
		_float				fRange;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	} LIGHTDESC;

	typedef struct ENGINE_DLL tagVertexPositionTexture_Declaration
	{
		static const unsigned int				iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	} VTXTEX_DECLARATION;

	typedef struct ENGINE_DLL tagVertexpositionNormalTexture_Declaration
	{
		static const unsigned int				iNumElements = 3;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	} VTXNORTEX_DECLARATION;
}