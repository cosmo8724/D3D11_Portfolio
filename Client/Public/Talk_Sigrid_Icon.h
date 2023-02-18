#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_Sigrid_Icon final : public CUI
{
private:
	CTalk_Sigrid_Icon(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_Sigrid_Icon(const CTalk_Sigrid_Icon& rhs);
	virtual ~CTalk_Sigrid_Icon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bFixPosition = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_Sigrid_Icon*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END