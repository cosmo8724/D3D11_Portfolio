#pragma once
#include "Client_Define.h"
#include "GameObject.h"

#define	INIT_LIGHT_COLOR	{ 1.f, 0.8f, 0.2f, 1.f }

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Sphere;
class CTexture;
class CShader;
END

BEGIN(Client)

class CSkyBox final : public CGameObject
{
private:
	CSkyBox(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CSkyBox(const CSkyBox& rhs);
	virtual ~CSkyBox() = default;

public:
	void						Set_LightDecrease(_bool bState) { m_bLightDecrease = bState; }
	void						Set_Lightning(_double dMaxTime = 0.2)
	{
		m_bLightning = true;
		m_dCurLightningTime = 0.0;
		m_dMaxLightningTime = dMaxTime;
		m_vLightDiffuse = INIT_LIGHT_COLOR;
	}

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Reflect() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Sphere*		m_pVIBufferCom = nullptr;

private:
	_float4					m_vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_bool						m_bLightDecrease = false;
	_bool						m_bLightning = false;
	_double					m_dCurLightningTime = 0.0;
	_double					m_dMaxLightningTime = 0.0;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();
	HRESULT					SetUp_ShaderResource_Reflect();

	void						Lightning();

public:
	static CSkyBox*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END