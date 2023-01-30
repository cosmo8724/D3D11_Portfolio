#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_Menu_Cloth final : public CUI
{
private:
	CShop_Menu_Cloth(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_Menu_Cloth(const CShop_Menu_Cloth& rhs);
	virtual ~CShop_Menu_Cloth() = default;

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
	static CShop_Menu_Cloth*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END