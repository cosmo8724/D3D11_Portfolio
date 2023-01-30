#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_BackGround final : public CUI
{
private:
	CShop_BackGround(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_BackGround(const CShop_BackGround& rhs);
	virtual ~CShop_BackGround() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	vector<CUI*>				m_vecChild;
	class CShop_Menu_Cloth*		m_pMenuCloth = nullptr;
	class CShop_Menu_Hair*		m_pMenuHair = nullptr;
	class CShop_Menu_Hat*			m_pMenuHat = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_BackGround*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END