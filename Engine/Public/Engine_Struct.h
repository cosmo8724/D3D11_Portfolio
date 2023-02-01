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

	typedef struct tagStatus
	{
		_int			iHP;
		_int			iMaxHP;
		_int			iMonsterDrink_Black;
		_int			iMonsterDrink_Orange;
		_int			iMonsterDrink_Pink;
		_int			iMonsterDrink_White;
		_int			iAttack;
		_int			iSpecialAttack;
		_double		dInitAttackCoolTime;
		_double		dCurAttackCoolTime;
		_double		dInitHitCoolTime;
		_double		dCurHitCoolTime;
	} STATUS;

	typedef struct tagVertexPosition
	{
		XMFLOAT3	vPosition;
	} VTXPOS;

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

	typedef struct tagVertexModelTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	} VTXMODEL;

	typedef struct tagVertexAnimModelTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
	} VTXANIMMODEL;

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

	typedef struct tagKeyFrame
	{
		double				dTime;
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
	} KEYFRAME;

	typedef  struct tagModelMaterial
	{
		class CTexture*	pTexture[AI_TEXTURE_TYPE_MAX];
	} MODELMATERIAL;

	typedef struct ENGINE_DLL tagVertexPosition_Declaration
	{
		static const unsigned int				iNumElements = 1;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	} VTXPOS_DECLARATION;

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

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int				iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	} VTXMODEL_DECLARATION;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int				iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;
}