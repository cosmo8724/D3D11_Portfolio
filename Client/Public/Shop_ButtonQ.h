#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_ButtonQ final : public CUI
{
private:
	CShop_ButtonQ(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_ButtonQ(const CShop_ButtonQ& rhs);
	virtual ~CShop_ButtonQ() = default;

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
	HRESULT					SetUp_ShaderResources();

public:
	static CShop_ButtonQ*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END