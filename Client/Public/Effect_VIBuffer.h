#pragma once
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_VIBuffer abstract : public CEffect
{
protected:
	CEffect_VIBuffer(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_VIBuffer(const CEffect_VIBuffer& rhs);
	virtual ~CEffect_VIBuffer() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() {}

protected:
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END