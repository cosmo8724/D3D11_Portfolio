#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instancing(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual HRESULT		Tick(_double dTimeDelta);
	virtual HRESULT		Render() override;

protected:
	_uint					m_iNumInstance = 0;
	_uint					m_iInstanceStride = 0;
	_uint					m_iIndexCountPerInstance = 0;
	ID3D11Buffer*		m_pInstanceBuffer = nullptr;

public:
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) PURE;
	virtual void			Free() override;
};

END