#pragma once
#include "UI.h"

BEGIN(Client)

class CShop_Icon_Cloth final : public CUI
{
private:
	CShop_Icon_Cloth(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShop_Icon_Cloth(const CShop_Icon_Cloth& rhs);
	virtual ~CShop_Icon_Cloth() = default;

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
	static CShop_Icon_Cloth*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END