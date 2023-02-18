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
	const _bool&				Get_Finished() const { return m_bFinish; }
	void						Set_Color(_float4 vColor) { m_vColor = vColor; }

public:
	virtual HRESULT			Initialize_Prototype(_uint iSpriteIndex, _bool bReversePlay);
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	CScene_Change*			m_pNextChanger = nullptr;

	_double					m_dTimeDelta = 0.0;
	_double					m_dSpeed = 0.02;
	_uint						m_iSpriteIndex = 0;
	_bool						m_bReversePlay = false;
	_bool						m_bColorReverse = false;
	_bool						m_bFinish = false;

	_int						m_iWidthFrame = 0;
	_int						m_iHeightFrame = 0;
	_int						m_iWidthCount = 0;
	_int						m_iHeightCount = 0;
	_float4					m_vColor = { 0.3f, 0.3f, 1.f, 1.f };

	_double					m_dEndTime = 0.0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResources();

public:
	static CScene_Change*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iSpriteIndex, _bool bReversePlay = false);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END