#pragma once
#include "Effect.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CEffect_Mesh abstract : public CEffect
{
protected:
	CEffect_Mesh(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_Mesh(const CEffect_Mesh& rhs);
	virtual ~CEffect_Mesh() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() {}

protected:
	CModel*					m_pModelCom = nullptr;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END