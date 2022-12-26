#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
private:
	CVIBuffer_Cell(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT			Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	static CVIBuffer_Cell*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3* pPoints);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END