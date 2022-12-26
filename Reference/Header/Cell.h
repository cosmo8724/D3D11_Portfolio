#pragma once
#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };

private:
	CCell(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CCell() = default;

public:
	_float3					Get_Point(POINT ePoint) { return m_vPoint[ePoint]; }
	void						Set_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) { m_iNeighborIndex[eNeighbor] = pNeighbor->m_iIndex; }

public:
	HRESULT					Initialize(const _float3* pPoints, _int iIndex);

public:
	_bool						Compare_Point(const _float3& SourPoint, const _float3& DestPoint);
	_bool						IsIn(_fvector vTargetPos, _int& iNeighborIndex);

#ifdef _DEBUG
public:
	HRESULT					Render(class CShader* pShader);
#endif // _DEBUG


private:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

private:
	_int						m_iIndex = 0;
	_float3					m_vPoint[POINT_END] = { _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f) };
	_int						m_iNeighborIndex[NEIGHBOR_END] = { -1, -1, -1 };

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*	m_pVIBufferCom = nullptr;
#endif // _DEBUG


public:
	static CCell*				Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3* pPoints, _int iIndex);
	virtual void				Free() override;
};

END