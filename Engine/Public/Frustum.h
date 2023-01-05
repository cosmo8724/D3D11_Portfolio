#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)

private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT			Initialize();
	void				TransformToWorldSpace();
	void				TransformToLocalSpace(_fmatrix matWorld);

public:
	_bool				IsInFrustum_World(_fvector vWorldPos, _float fRange);
	_bool				IsInFrustum_Local(_fvector vLocalPos, _float fRange);

private:
	_float3			m_vPoint[8];
	_float3			m_vPointInWorld[8];

	_float4			m_vPlaneInWorld[6];
	_float4			m_vPlaneInLocal[6];

private:
	void				Build_Planes(_float4* pPlanes, const _float3* pPoints);

public:
	virtual void		Free() override;
};

END