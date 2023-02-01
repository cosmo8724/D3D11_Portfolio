#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_Menu_Hat final : public CUI
{
private:
	CShop_Menu_Hat(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_Menu_Hat(const CShop_Menu_Hat& rhs);
	virtual ~CShop_Menu_Hat() = default;

public:
	vector<CUI*>*				Get_Childs() { return &m_vecChild; }
	void						Set_Selected(_bool bSelected) { m_bSelected = bSelected; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	vector<CUI*>				m_vecChild;
	_bool						m_bFixPosition = false;
	_bool						m_bSelected = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_Menu_Hat*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END