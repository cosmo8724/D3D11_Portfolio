#pragma once
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_PointInstancing;
END

BEGIN(Client)

class CEffect_Trail abstract : public CEffect
{
protected:
	CEffect_Trail(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

public:
	void						Set_Owner(CGameObject* pGameObject) { m_pOwner = pGameObject; }
	void						Set_WorldMatrix(_float4x4 matWorld) { m_pTransformCom->Set_WorldMatrix(matWorld); }
	void						Set_Active(_bool bActive) { m_bActive = bActive; }
	void						Set_Color(_float4 vColor) { m_vColor = vColor; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty();

protected:
	CVIBuffer_PointInstancing*		m_pVIBufferCom = nullptr;
	CGameObject*			m_pOwner = nullptr;

	_bool						m_bActive = true;
	_bool						m_bAlpha = false;
	_float						m_fLife = 0.f;
	_float						m_fWidth = 0.f;
	_float						m_fSegmentSize = 0.f;
	_float4					m_vColor = { 1.f, 1.f, 1.f, 1.f };
	vector<_float4>			m_vecPrePositions;

protected:
	virtual HRESULT			SetUp_Component();
	virtual HRESULT			SetUp_ShaderResource();

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END