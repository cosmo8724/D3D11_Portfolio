#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_NPC_Icon final : public CUI
{
private:
	CTalk_NPC_Icon(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_NPC_Icon(const CTalk_NPC_Icon& rhs);
	virtual ~CTalk_NPC_Icon() = default;

public:
	const _int	&				Get_NPCIndex() const { return m_iNPCIndex; }
	void						Set_NPCIndex(_int iIndex) { m_iNPCIndex = iIndex; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bFixPosition = false;
	_int						m_iNPCIndex = -1;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_NPC_Icon*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END