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

	if (iWidth > 10000)
	{
		m_iWidth = iWidth;
		m_iHeight = iHeight;
		
		FAILED_CHECK_RETURN(Ready_DepthStencilRenderTargetView(m_iWidth, m_iHeight), E_FAIL);
	}

	m_ePixelFormat = ePixelFormat;
	m_vClearColor = vClearColor;

	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));
	m_ViewPort.TopLeftX = 0.f;
	m_ViewPort.TopLeftY = 0.f;
	m_ViewPort.Width = (_float)iWidth;
	m_ViewPort.Height = (_float)iHeight;
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;

	return S_OK;
}

HRESULT CRenderTarget::Tick(_uint iWidth, _uint iHeight)
{
	if (m_iWidth > 10000)
		return S_FALSE;

	if (m_pTexture2D != nullptr)
		Safe_Release(m_pTexture2D);

	if (m_pRenderTargetView != nullptr)
		Safe_Release(m_pRenderTargetView);

	if (m_pShaderResourceView != nullptr)
		Safe_Release(m_pShaderResourceView);

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	if (m_iWidth > 10000)
	{
		TextureDesc.Width = m_iWidth;
		TextureDesc.Height = m_iHeight;
	}
	else
	{
		TextureDesc.Width = iWidth;
		TextureDesc.Height = iHeight;
	}

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

HRESULT CRenderTarget::Ready_DepthStencilRenderTargetView(_uint iWidth, _uint iHeight)
{
	NULL_CHECK_RETURN(m_pDevice, E_FAIL);

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture), E_FAIL);
	FAILED_CHECK_RETURN(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView), E_FAIL);

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

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
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pTexture2D);
}
