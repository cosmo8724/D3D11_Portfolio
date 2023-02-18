#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_Sigrid_Background final : public CUI
{
private:
	CTalk_Sigrid_Background(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_Sigrid_Background(const CTalk_Sigrid_Background& rhs);
	virtual ~CTalk_Sigrid_Background() = default;

public:
	void						Set_Sentences(const wstring& wstrSentence1, const wstring& wstrSentence2) {
		m_wstrSentences[0] = wstrSentence1;
		m_wstrSentences[1] = wstrSentence2;
	}

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	vector<CUI*>				m_vecChild;
	wstring					m_wstrSentences[2] = { L"" , L"" };

	class CTalk_Button1*	m_pButton1 = nullptr;
	class CTalk_Button2*	m_pButton2 = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_Sigrid_Background*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END