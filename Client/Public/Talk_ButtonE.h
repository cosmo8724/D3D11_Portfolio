#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_ButtonE final : public CUI
{
private:
	CTalk_ButtonE(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_ButtonE(const CTalk_ButtonE& rhs);
	virtual ~CTalk_ButtonE() = default;

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
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_ButtonE*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END