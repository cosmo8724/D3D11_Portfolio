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
	, m_vecTexture(rhs.m_vecTexture)
{
	for (auto& pTexture : m_vecTexture)
		Safe_AddRef(pTexture);
}

HRESULT CTexture::Initialize_Prototype(const wstring wstrTextureFilePath, _uint iNumTexture)
{
	m_vecTexture.reserve(iNumTexture);

	m_iNumTexture = iNumTexture;

	for (_uint i = 0; i < m_iNumTexture; ++i)
	{
		ID3D11ShaderResourceView*		pSRV = nullptr;

		_tchar		szTexturePath[MAX_PATH] = L"";

		wsprintf(szTexturePath, wstrTextureFilePath.c_str(), i);

		_tchar		szExt[MAX_PATH] = L"";

		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT	hr = 0;
		if (!lstrcmp(szExt, L".tga"))
			hr = E_FAIL;
		else if (!lstrcmp(szExt, L".dds"))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, &pSRV);
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &pSRV);
		
		FAILED_CHECK_RETURN(hr, E_FAIL);

		m_vecTexture.push_back(pSRV);
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

	return pShaderCom->Set_ShaderResourceView(wstrConstantName, m_vecTexture[iTextureIndex]);
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

	for (auto& pTexture : m_vecTexture)
		Safe_Release(pTexture);

	m_vecTexture.clear();
}
