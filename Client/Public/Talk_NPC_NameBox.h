#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_NPC_NameBox final : public CUI
{
private:
	CTalk_NPC_NameBox(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_NPC_NameBox(const CTalk_NPC_NameBox& rhs);
	virtual ~CTalk_NPC_NameBox() = default;

public:
	const wstring&			Get_Name() const { return m_wstrName; }
	void						Set_Name(const wstring& wstrName) { m_wstrName = wstrName; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bFixPosition = false;
	wstring					m_wstrName = L"";

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_NPC_NameBox*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END