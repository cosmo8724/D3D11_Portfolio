#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_Button2 final : public CUI
{
private:
	CShop_Button2(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_Button2(const CShop_Button2& rhs);
	virtual ~CShop_Button2() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	wstring					m_wstrNo = L"³ª°¡±â";
	_bool						m_bFixPosition = false;
	_bool						m_bMouseHover = false;
	_bool						m_bReadyToDead = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_Button2*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END