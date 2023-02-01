#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_ItemSelect final : public CUI
{
public:
	enum FRAME { FRAME_LEFT, FRAME_RIGHT, FRAME_END };

private:
	CShop_ItemSelect(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_ItemSelect(const CShop_ItemSelect& rhs);
	virtual ~CShop_ItemSelect() = default;

public:
	void						Set_ItemSlot(_int iItemSlot) { m_iCurItemSlot = iItemSlot; }

public:
	virtual HRESULT			Initialize_Prototype(FRAME eFrame);
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	FRAME						m_eFrame = FRAME_END;
	class CShop_ItemBar*	m_pCurItemBar = nullptr;

	_bool						m_bFixPosition = false;
	_int						m_iCurItemSlot = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_ItemSelect*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, FRAME eFrame);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END