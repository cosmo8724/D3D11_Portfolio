#pragma once
#include "UI.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CHPBar_Leviathan final : public CUI
{
private:
	CHPBar_Leviathan(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CHPBar_Leviathan(const CHPBar_Leviathan& rhs);
	virtual ~CHPBar_Leviathan() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	CModel*					m_pModelCom = nullptr;
	CTexture*					m_pFrameTexture = nullptr;
	CTexture*					m_pHPTexture = nullptr;

	class CLeviathan*		m_pLeviathan = nullptr;

private:
	_int						m_iMaxHP = 0;;
	_int						m_iCurHP = 0;
	_float						m_fHPRatio = 0.f;
	_float						m_fGlobalRatio = 0.f;
	_bool						m_bReady = false;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CHPBar_Leviathan*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END