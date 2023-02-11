#pragma once
#include "Effect_Mesh.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CEffect_Cocoball final : public CEffect_Mesh
{
private:
	CEffect_Cocoball(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_Cocoball(const CEffect_Cocoball& rhs);
	virtual ~CEffect_Cocoball() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	class CSigrid*				m_pPlayer = nullptr;
	CCollider*					m_pSphereCol = nullptr;

	class CTrail_Cocoball*	m_pTrail = nullptr;

	_float4					m_vDir;
	_bool						m_bFire = false;
	_bool						m_bTurn = false;
	_float						m_fAlpha = 0.f;
	_double					m_dLifeTime = 0.0;

	_bool						m_bScaleFix = false;
	_double					m_dTimeDelta = 0.0;
	_double					m_dSpeed = 0.025;
	_bool						m_bFinish = false;
	_int						m_iWidthFrame = 0;
	_int						m_iHeightFrame = 0;
	_int						m_iWidthCount = 0;
	_int						m_iHeightCount = 0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CEffect_Cocoball*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END