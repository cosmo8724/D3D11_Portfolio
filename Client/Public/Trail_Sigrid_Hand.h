#pragma once
#include "Effect_Trail.h"

BEGIN(Client)

class CTrail_Sigrid_Hand final : public CEffect_Trail
{
private:
	CTrail_Sigrid_Hand(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTrail_Sigrid_Hand(const CTrail_Sigrid_Hand& rhs);
	virtual ~CTrail_Sigrid_Hand() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	virtual HRESULT			SetUp_Component() override;
	virtual HRESULT			SetUp_ShaderResource() override;

public:
	static CTrail_Sigrid_Hand*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END