#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTargetMgr final : public CBase
{
	DECLARE_SINGLETON(CRenderTargetMgr)

private:
	CRenderTargetMgr() = default;
	virtual ~CRenderTargetMgr() = default;

public:
	ID3D11RenderTargetView* 		Get_RenderTargetView(const wstring& wstrTargetTag);
	ID3D11ShaderResourceView*		Get_ShaderResourceView(const wstring& wstrTargetTag);
	ID3D11RenderTargetView**		Get_BackBufferRenderTargetView() { return &m_pBackBufferView; }
	ID3D11DepthStencilView*		Get_DepthStencilView() { return m_pDepthStencilView; }

public:
	HRESULT							Initialize(DEVICE pDevice, DEVICE_CONTEXT pContext);
	void								Tick(_double dTimeDelta);
	HRESULT							Add_RenderTarget(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT							Add_MultiRenderTarget(const wstring& wstrMultiTargetTag, const wstring& wstrTargetTag);

	HRESULT							Begin_RenderTarget(DEVICE_CONTEXT pDevice, const wstring& wstrTargetTag);
	HRESULT							Begin_ShadowDepthRenderTarget(DEVICE_CONTEXT pDevice, const wstring& wstrTargetTag);
	HRESULT							Begin_MultiRenderTarget(DEVICE_CONTEXT pContext, const wstring& wstrMultiTargetTag);
	HRESULT							End_MultiRenderTarget(DEVICE_CONTEXT pContext, const wstring& wstrMultiTargetTag);

#ifdef _DEBUG
public:
	HRESULT							Ready_Debug(const wstring& wstrTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT							Render_Debug(const wstring& wstrMultiTargetTag);
#endif // _DEBUG

private:
	DEVICE_CONTEXT				m_pContext = nullptr;

	ID3D11RenderTargetView*		m_pBackBufferView = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;
	D3D11_VIEWPORT				m_ViewPortOrigin;

private:
	map<const wstring, class CRenderTarget*>			m_mapRenderTarget;
	typedef map<const wstring, class CRenderTarget*>	RENDERTARGETS;

	map<const wstring, list<class CRenderTarget*>>		m_mapMultiRenderTarget;
	typedef map<const wstring, list<class CRenderTarget*>>	MULTIRENDERTARGETS;

#ifdef _DEBUG
private:
	class CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	class CShader*					m_pShaderCom = nullptr;
	_float4x4							m_matView, m_matProj;
#endif // _DEBUG

private:
	class CRenderTarget*			Find_RenderTarget(const wstring& wstrTargetTag);
	list<class CRenderTarget*>*	Find_MultiRenderTarget(const wstring& wstrMultiTargetTag);

public:
	virtual void					Free() override;
};

END