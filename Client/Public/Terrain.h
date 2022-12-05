#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Terrain;

END

BEGIN(Client)

class CTerrain final : public CGameObject
{
	enum TEXTURETYPE { TEXTURE_DIFFUSE, TEXTURE_BRUSH, TEXTURE_FILTER, TEXTURETYPE_END };
private:
	CTerrain(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;
public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void			Tick(_double dTimeDelta) override;
	virtual void			Late_Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom[TEXTURETYPE_END] = { nullptr };
	CVIBuffer_Terrain*	m_pVIBufferCom = nullptr;

private:
	HRESULT				SetUp_Component();
	HRESULT				SetUp_ShaderResource();

public:
	static CTerrain*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void			Free() override;
};

END