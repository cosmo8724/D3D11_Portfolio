#pragma once
#include "UI.h"

BEGIN(Client)

class CTalk_NPC_Corner final : public CUI
{
private:
	CTalk_NPC_Corner(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTalk_NPC_Corner(const CTalk_NPC_Corner& rhs);
	virtual ~CTalk_NPC_Corner() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_bool						m_bFixPosition = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_Parts();
	HRESULT					SetUp_ShaderResources();

public:
	static CTalk_NPC_Corner*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END