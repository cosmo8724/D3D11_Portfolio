#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_ButtonS final : public CUI
{
private:
	CShop_ButtonS(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_ButtonS(const CShop_ButtonS& rhs);
	virtual ~CShop_ButtonS() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bFixPosition = false;
	_bool						m_bMouseHover = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_ButtonS*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END