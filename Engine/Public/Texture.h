#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView*	Get_Texture(_uint iTextureIndex = 0) const { return m_pTextures[iTextureIndex]; }

public:
	virtual HRESULT			Initialize_Prototype(const wstring wstrTextureFilePath, _uint iNumTexture);
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				ImGui_RenderProperty() override;


public:
	HRESULT					Bind_ShaderResource(class CShader* pShaderCom, const wstring wstrConstantName, _uint iTextureIndex = 0);
	HRESULT					Bind_ShaderResources(class CShader* pShaderCom, const wstring wstrConstantName);

private:
	_uint												m_iNumTexture = 0;
	ID3D11ShaderResourceView**					m_pTextures;

public:
	static CTexture*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring wstrTextureFilePath, _uint iNumTexture = 1);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END