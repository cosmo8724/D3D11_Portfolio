#pragma once
#include "Effect_Mesh.h"

BEGIN(Client)

class CEffect_GroundSlam final : public CEffect_Mesh
{
public:
	CEffect_GroundSlam(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_GroundSlam(const CEffect_GroundSlam& rhs);
	virtual ~CEffect_GroundSlam() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	_double					m_dLimitTime = 0.0;
	_bool						m_bScaleFix = false;
	_float						m_fFadeAlpha = 1.f;

	_double					m_dTimeDelta = 0.0;
	_double					m_dSpeed = 0.02;
	_uint						m_iSpriteIndex = 0;
	_bool						m_bFinish = false;
	_uint						m_iWidthFrame = 0;
	_uint						m_iHeightFrame = 0;
	_uint						m_iWidthCount = 0;
	_uint						m_iHeightCount = 0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CEffect_GroundSlam*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END