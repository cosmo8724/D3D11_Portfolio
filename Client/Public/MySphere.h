#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CVIBuffer_Sphere;
class CTexture;
END

BEGIN(Client)

class CMySphere final : public CGameObject
{
private:
	CMySphere(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMySphere(const CMySphere& rhs);
	virtual ~CMySphere() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			Tick(_double dTimeDelta) override;
	virtual void			Late_Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CVIBuffer_Sphere*	m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT				SetUp_Component();
	HRESULT				SetUp_ShaderResource();

public:
	static	CMySphere*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};

END