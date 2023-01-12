#include "stdafx.h"
#include "..\Public\RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D), E_FAIL);

	FAILED_CHECK_RETURN(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRenderTargetView), E_FAIL);

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pShaderResourceView), E_FAIL);

	m_ePixelFormat = ePixelFormat;
	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Tick(_uint iWidth, _uint iHeight)
{
	if (m_pTexture2D != nullptr)
		Safe_Release(m_pTexture2D);

	if (m_pRenderTargetView != nullptr)
		Safe_Release(m_pRenderTargetView);

	if (m_pShaderResourceView != nullptr)
		Safe_Release(m_pShaderResourceView);

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = m_ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D), E_FAIL);

	FAILED_CHECK_RETURN(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRenderTargetView), E_FAIL);

	FAILED_CHECK_RETURN(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pShaderResourceView), E_FAIL);

	return S_OK;
}

HRESULT CRenderTarget::Clear_RenderTarget()
{
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT		ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	_uint	iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewPortDesc);

	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());

	m_matWorld._11 = fSizeX;
	m_matWorld._22 = fSizeY;
	m_matWorld._41 = fX - ViewPortDesc.Width * 0.5f;
	m_matWorld._42 = -fY + ViewPortDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader * pShaderCom, CVIBuffer_Rect * pVIBufferCom)
{
	NULL_CHECK_RETURN(pShaderCom, E_FAIL);
	NULL_CHECK_RETURN(pVIBufferCom, E_FAIL);

	pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorld);
	pShaderCom->Set_ShaderResourceView(L"g_Texture", m_pShaderResourceView);

	pShaderCom->Begin(0);
	pVIBufferCom->Render();

	return S_OK;
}
#endif //_DEBUG

CRenderTarget * CRenderTarget::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iWidth, iHeight, ePixelFormat, vClearColor)))
	{
		MSG_BOX("Failed to Create : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pShaderResourceView);
	Safe_Release(m_pTexture2D);
}
