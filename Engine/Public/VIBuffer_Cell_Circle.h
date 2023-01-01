#pragma once
#include "Engine_Define.h"
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell_Circle final : public CVIBuffer
{
private:
	CVIBuffer_Cell_Circle(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_Cell_Circle(const CVIBuffer_Cell_Circle& rhs);
	virtual ~CVIBuffer_Cell_Circle() = default;

public:
	virtual HRESULT					Initialize_Prototype(const _float3& vPoint);
	virtual HRESULT					Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	static CVIBuffer_Cell_Circle*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3& vPoint);
	virtual CComponent*			Clone(class CGameObject* pOwner, void* pArg = nullptr);
	virtual void						Free() override;

};

END