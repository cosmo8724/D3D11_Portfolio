#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_ItemBar final : public CUI
{
public:
	enum ITEMBAR_STATE { ITEMBAR_EQUIPED, ITEMBAR_NOTPURCHASED, ITEMBAR_PURCHASED, ITEMBAR_END };

	typedef struct tagItemBarDesc : public CGameObject::GAMEOBJECTDESC
	{
		wstring		wstrUITag;
		wstring		wstrItemName;
		_uint			iSlot = 0;
	} ITEMBARDESC;

private:
	CShop_ItemBar(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_ItemBar(const CShop_ItemBar& rhs);
	virtual ~CShop_ItemBar() = default;

public:
	void						Set_Selected(_bool bSelected) { m_bSelected = bSelected; }
	void						Set_ItemBarStatus(ITEMBAR_STATE eState) { m_eState = eState; }

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bFixPosition = false;
	vector<CUI*>				m_vecChild;

	ITEMBAR_STATE			m_eState = ITEMBAR_NOTPURCHASED;
	_bool						m_bSelected = false;

	wstring					m_wstrItemName = L"";
	wstring					m_wstrState = L"";
	_uint						m_iMenu = 0;
	_uint						m_iSlot = 0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_ItemBar*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END