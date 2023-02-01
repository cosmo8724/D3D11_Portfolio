#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_Button1 final : public CUI
{
private:
	CShop_Button1(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_Button1(const CShop_Button1& rhs);
	virtual ~CShop_Button1() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	wstring					m_wstrYes = L"";
	_bool						m_bFixPosition = false;
	_bool						m_bMouseHover = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_Button1*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END