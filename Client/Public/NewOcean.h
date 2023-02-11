#pragma once
#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_PointInstancing;
END

BEGIN(Client)

class CNewOcean final : public CGameObject
{
private:
	CNewOcean(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CNewOcean(const CNewOcean& rhs);
	virtual ~CNewOcean() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				ImGui_RenderProperty() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CTexture*					m_pFoamTextureCom = nullptr;
	CTexture*					m_pHeightTextureCom = nullptr;
	CVIBuffer_PointInstancing*		m_pVIBufferCom = nullptr;

private:
	_double					m_dTimeDelta = 0.0;
	_float3					m_vWaterColor = { 0.27f, 0.74f, 1.f };
	_float						m_fWaterSpeed = 400.f;
	_float						m_fWaterLevel = 0.f;
	_float						m_fWaveLength = 0.1f;
	_float						m_fWaveHeight = 0.4f;
	_float						m_fWindForce = -20.f;
	_float						m_fSpecularPerturb = 4.f;
	_float						m_fSpecularPower = 364.f;
	_float						m_fDullBlendFactor = 0.3f;

	_float4x4					m_matReflectionView;
	_float4					m_vClipPlane[2];

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	void						Update_ReflectionCam();
	void						Update_ClipPlane();

public:
	static CNewOcean*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END