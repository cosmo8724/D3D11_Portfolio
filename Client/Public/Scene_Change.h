#pragma once
#include "UI.h"

BEGIN(Client)

class CScene_Change final : public CUI
{
private:
	CScene_Change(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CScene_Change(const CScene_Change& rhs);
	virtual ~CScene_Change() = default;

public:
	virtual HRESULT			Initialize_Prototype(_uint iSpriteIndex, _bool bReversePlay);
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_double					m_dTimeDelta = 0.0;
	_double					m_dSpeed = 0.02;
	_uint						m_iSpriteIndex = 0;
	_bool						m_bReversePlay = false;
	_bool						m_bColorReverse = false;
	_bool						m_bFinish = false;

	_uint						m_iWidthFrame = 0;
	_uint						m_iHeightFrame = 0;
	_uint						m_iWidthCount = 0;
	_uint						m_iHeightCount = 0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CScene_Change*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iSpriteIndex, _bool bReversePlay = false);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END