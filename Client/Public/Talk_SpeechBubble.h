#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_SpeechBubble final : public CUI
{
private:
	CTalk_SpeechBubble(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_SpeechBubble(const CTalk_SpeechBubble& rhs);
	virtual ~CTalk_SpeechBubble() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	vector<CUI*>				m_vecChild;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_SpeechBubble*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END