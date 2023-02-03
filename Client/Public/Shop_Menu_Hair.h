#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_Menu_Hair final : public CUI
{
private:
	CShop_Menu_Hair(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_Menu_Hair(const CShop_Menu_Hair& rhs);
	virtual ~CShop_Menu_Hair() = default;

public:
	vector<CUI*>*				Get_Childs() { return &m_vecChild; }
	_uint						Get_CurEquipSlot() { return m_iCurEquipSlot; }
	const _bool&				Is_Selected() const { return m_bSelected; }
	void						Set_Selected(_bool bSelected) { m_bSelected = bSelected; }
	void						Set_CurEquipSlot(_uint iSlot) { m_iCurEquipSlot = iSlot; }

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
	_uint						m_iCurEquipSlot = 0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_Menu_Hair*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END