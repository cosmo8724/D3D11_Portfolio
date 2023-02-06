#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstancing final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_PointInstancing(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_PointInstancing(const CVIBuffer_PointInstancing& rhs);
	virtual ~CVIBuffer_PointInstancing() = default;

public:
	virtual HRESULT		Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT		Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual HRESULT		Tick(_double dTimeDelta);
	virtual HRESULT		Render() override;

public:
	static CVIBuffer_PointInstancing*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iNumInstance = 1);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END