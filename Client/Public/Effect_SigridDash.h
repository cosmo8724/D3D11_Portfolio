#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CEffect_SigridDash final : public CEffect_Mesh
{
private:
	CEffect_SigridDash(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_SigridDash(const CEffect_SigridDash& rhs);
	virtual ~CEffect_SigridDash() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_double					m_dLimitTime = 0.0;
	_bool						m_bScaleFix = false;
	_float						m_fFadeAlpha = 1.f;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CEffect_SigridDash*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END