#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	static CVIBuffer_Rect*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr);
	virtual void				Free() override;
};

END