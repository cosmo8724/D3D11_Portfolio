#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_Button1 final : public CUI
{
private:
	CTalk_Button1(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_Button1(const CTalk_Button1& rhs);
	virtual ~CTalk_Button1() = default;

public:
	const wstring&			Get_Sentence() const { return m_wstrSentence; }
	void						Set_Sentence(const wstring& wstrSentence) { m_wstrSentence = wstrSentence; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	wstring					m_wstrSentence = L"";
	_bool						m_bFixPosition = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_Button1*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END