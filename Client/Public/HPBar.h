#pragma once
#include "UI.h"

BEGIN(Client)

class CHPBar final : public CUI
{
private:
	CHPBar(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CHPBar(const CHPBar& rhs);
	virtual ~CHPBar() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	wstring					m_wstrHP = L"";

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CHPBar*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END