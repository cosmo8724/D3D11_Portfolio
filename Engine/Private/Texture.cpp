#include "..\Public\Texture.h"
#include "GameUtility.h"
#include "Shader.h"

CTexture::CTexture(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTexture(rhs.m_iNumTexture)
	, m_pTextures(rhs.m_pTextures)
{
	for (_uint i = 0; i < m_iNumTexture; ++i)
		Safe_AddRef(m_pTextures[i]);
}

HRESULT CTexture::Initialize_Prototype(const wstring wstrTextureFilePath, _uint iNumTexture)
{
	m_pTextures = new ID3D11ShaderResourceView*[iNumTexture];

	m_iNumTexture = iNumTexture;

	for (_uint i = 0; i < m_iNumTexture; ++i)
	{
		_tchar		szTexturePath[MAX_PATH] = L"";

		wsprintf(szTexturePath, wstrTextureFilePath.c_str(), i);

		_tchar		szExt[MAX_PATH] = L"";

		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT	hr = 0;
		if (!lstrcmp(szExt, L".tga"))
			hr = E_FAIL;
		else if (!lstrcmp(szExt, L".dds"))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);
		
		FAILED_CHECK_RETURN(hr, E_FAIL);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShaderCom, const wstring wstrConstantName, _uint iTextureIndex)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);
	assert(iTextureIndex < m_iNumTexture);

	return pShaderCom->Set_ShaderResourceView(wstrConstantName, m_pTextures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader * pShaderCom, const wstring wstrConstantName)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);

	return pShaderCom->Set_ShaderResourceViewArray(wstrConstantName, m_pTextures, m_iNumTexture);
}

CTexture * CTexture::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring wstrTextureFilePath, _uint iNumTexture)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrTextureFilePath, iNumTexture)))
	{
		MSG_BOX("Failed to Create : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumTexture; ++i)
		Safe_Release(m_pTextures[i]);

	if (m_bIsCloned == false)
		Safe_Delete_Array(m_pTextures);
}
