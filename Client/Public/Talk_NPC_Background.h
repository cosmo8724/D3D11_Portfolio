#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_NPC_Background final : public CUI
{
private:
	CTalk_NPC_Background(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_NPC_Background(const CTalk_NPC_Background& rhs);
	virtual ~CTalk_NPC_Background() = default;

public:
	void						Set_Owner(class CNPC* pNPC) { m_pOwner = pNPC; }
	void						Set_Name(const wstring& wstrName) { m_wstrName = wstrName; }
	void						Set_Sentence(vector<wstring>* pSentences) { m_pSentences = pSentences; }
	void						Set_Answer(_int iAnswer) { m_iAnswer = iAnswer; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	vector<CUI*>				m_vecChild;
	class CNPC*				m_pOwner = nullptr;
	class CTalk_NPC_NameBox*	m_pNameBox = nullptr;
	class CTalk_NPC_Icon*			m_pIcon = nullptr;

	wstring					m_wstrName = L"";
	vector<wstring>*			m_pSentences = nullptr;
	_uint						m_iCurSentence = 0;
	_bool						m_bTalkFinish = false;
	_bool						m_bReserveDead = false;
	_int						m_iAnswer = -1;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_NPC_Background*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END