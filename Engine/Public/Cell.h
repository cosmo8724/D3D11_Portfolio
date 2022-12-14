#pragma once
#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHTBOR_AB, NEIGHBOR_BC, NEIGHTBOR_CA, NEIGHTBOR_END };

private:
	CCell(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CCell() = default;

public:
	HRESULT					Initialize(const _float3* pPoints, _int iIndex);

private:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

private:
	_int						m_iIndex = 0;
	_float3					m_vPoint[POINT_END] = { _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f) };
	_int						m_iNeighborIndex[NEIGHTBOR_END] = { -1, -1, -1 };

public:
	static CCell*				Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3* pPoints, _int iIndex);
	virtual void				Free() override;
};

END