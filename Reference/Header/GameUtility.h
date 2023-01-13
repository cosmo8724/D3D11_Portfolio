#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CGameUtility
{
public:
	CGameUtility() = default;
	~CGameUtility() = default;

public:
	static void		wctc(const wchar_t* szWchar, char* szChar);
	static void		ctwc(const char* szChar, wchar_t* wszChar);
	static string		WstrToStr(const wstring& wstrSour);
	static void		SplitTag(wchar_t* wszTag, wstring& wszChar);	// Split Tag by '_'
	static void		ConvertNumberTo_d(char* szChar);
	static COMPONENTTYPE	CheckComponentTypeFromTag(const wstring& wstrComponentTag);

	static void		Saturate(int& InValue, int InMax, int InMin);
	static void		SaturateF(float& InValue, float InMax, float InMin);
	static void		DecomposePitchYawRollFromXMMatrix(_fmatrix matRotation, _float& fPitch, _float& fYaw, _float& fRoll);

	static pair<_bool, _float>	Picking(HWND& hWnd, _float fScreenWidth, _float fScreenHeight, class CTransform* pTransformCom, _float3* pVertexPos, _uint iNumVerticesX, _uint iNumVerticesZ, _float3& vPickingPoint);
	static pair<_bool, _float>	Picking(HWND& hWnd, _float fScreenWidth, _float fScreenHeight, class CTransform* pTransformCom, VTXMODEL* pVertices, FACEINDICES32* pIndices, _uint iNumPrimitive, _float3& vPickingPoint);
	static void		SortPointsByCW(_float3* vPoints);

	static	_bool		CollisionSphereSphere(class CCollider* pColliderA, class CCollider* pColliderB, _float4& vDir);
};

END