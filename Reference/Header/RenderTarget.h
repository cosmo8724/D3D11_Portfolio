#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView*		Get_RenderTargetView() const { return m_pRenderTargetView; }
	ID3D11ShaderResourceView*		Get_ShaderResourceView() const { return m_pShaderResourceView; }
	ID3D11DepthStencilView*		Get_DepthStencilView() const { return m_pDepthStencilView; }
	const D3D11_VIEWPORT&		Get_ViewPortDesc() const { return m_ViewPort; }

public:
	HRESULT							Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT							Tick(_uint iWidth, _uint iHeight);
	HRESULT							Clear_RenderTarget();

#ifdef _DEBUG
public:
	HRESULT							Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT							Render(class CShader* pShaderCom, class CVIBuffer_Rect* pVIBufferCom);
#endif // _DEBUG

private:
	DEVICE							m_pDevice = nullptr;
	DEVICE_CONTEXT				m_pContext = nullptr;

	DXGI_FORMAT					m_ePixelFormat;
	ID3D11Texture2D*				m_pTexture2D = nullptr;
	ID3D11RenderTargetView*		m_pRenderTargetView = nullptr;
	ID3D11ShaderResourceView*		m_pShaderResourceView = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
	D3D11_VIEWPORT				m_ViewPort;

	_float4							m_vClearColor;

	_uint								m_iWidth = 0;
	_uint								m_iHeight = 0;

#ifdef _DEBUG
private:
	_float4x4							m_matWorld;
#endif // _DEBUG

private:
	HRESULT							Ready_DepthStencilRenderTargetView(_uint iWidth, _uint iHeight);

public:
	static CRenderTarget*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void						Free() override;
};

END