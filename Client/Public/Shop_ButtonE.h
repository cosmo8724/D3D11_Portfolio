#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_ButtonE final : public CUI
{
private:
	CShop_ButtonE(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_ButtonE(const CShop_ButtonE& rhs);
	virtual ~CShop_ButtonE() = default;

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
	static CShop_ButtonE*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END