#include "stdafx.h"
#include "..\Public\RenderTargetMgr.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CRenderTargetMgr)

HRESULT CRenderTargetMgr::Initialize(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	m_pContext = pContext;
	Safe_AddRef(m_pContext);

#ifdef _DEBUG
	D3D11_VIEWPORT		ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	_uint		iNumViewport = 1;

	pContext->RSGetViewports(&iNumViewport, &ViewPortDesc);

	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));

	m_pVIBufferCom = CVIBuffer_Rect::Create(pDevice, pContext);
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	m_pShaderCom = CShader::Create(pDevice, pContext, L"../Bin/Shader/Shader_Deferred.hlsl", CShader::DECLARATION_VTXTEX, VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
#endif // _DEBUG

	return S_OK;
}

void CRenderTargetMgr::Tick(_double dTimeDelta)
{
	D3D11_VIEWPORT		ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	_uint		iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewPortDesc);

	for (auto& Pair : m_mapRenderTarget)
		Pair.second->Tick((_uint)ViewPortDesc.Width, (_uint)ViewPortDesc.Height);

#ifdef _DEBUG
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewPortDesc.Width, ViewPortDesc.Height, 0.f, 1.f));
#endif // _DEBUG
}

HRESULT CRenderTargetMgr::Add_RenderTarget(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	if (Find_RenderTarget(wstrTargetTag) != nullptr)
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pContext, iWidth, iHeight, ePixelFormat, vClearColor);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	m_mapRenderTarget.emplace(wstrTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTargetMgr::Add_MultiRenderTarget(const wstring & wstrMultiTargetTag, const wstring & wstrTargetTag)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(wstrTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	list<CRenderTarget*>*	pMultiRenderTargetList = Find_MultiRenderTarget(wstrMultiTargetTag);

	if (pMultiRenderTargetList == nullptr)
	{
		list<CRenderTarget*>	MultiRenderTargetList;

		MultiRenderTargetList.push_back(pRenderTarget);

		m_mapMultiRenderTarget.emplace(wstrMultiTargetTag, MultiRenderTargetList);
	}
	else
		pMultiRenderTargetList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTargetMgr::Begin_MultiRenderTarget(DEVICE_CONTEXT pContext, const wstring & wstrMultiTargetTag)
{
	list<CRenderTarget*>*	pMultiRenderTargetList = Find_MultiRenderTarget(wstrMultiTargetTag);
	NULL_CHECK_RETURN(pMultiRenderTargetList, E_FAIL);

	pContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView*	pRenderTargetView[8] = { nullptr };
	_uint	iNumViews = 0;

	for (auto& pRenderTarget : *pMultiRenderTargetList)
	{
		pRenderTarget->Clear_RenderTarget();
		pRenderTargetView[iNumViews++] = pRenderTarget->Get_RenderTargetView();
	}

	pContext->OMSetRenderTargets(iNumViews, pRenderTargetView, m_pDepthStencilView);

	return S_OK;
}

HRESULT CRenderTargetMgr::End_MultiRenderTarget(DEVICE_CONTEXT pContext, const wstring & wstrMultiTargetTag)
{
	pContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTargetMgr::Ready_Debug(const wstring & wstrTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget*		pRenderTarget = Find_RenderTarget(wstrTargetTag);
	NULL_CHECK_RETURN(pRenderTarget, E_FAIL);

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRenderTargetMgr::Render_Debug(const wstring & wstrMultiTargetTag)
{
	list<CRenderTarget*>*	pMultiRenderTargetList = Find_MultiRenderTarget(wstrMultiTargetTag);
	NULL_CHECK_RETURN(pMultiRenderTargetList, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);

	for (auto& pRenderTarget : *pMultiRenderTargetList)
		pRenderTarget->Render(m_pShaderCom, m_pVIBufferCom);

	return S_OK;
}
#endif // _DEBUG

CRenderTarget * CRenderTargetMgr::Find_RenderTarget(const wstring & wstrTargetTag)
{
	auto	iter = find_if(m_mapRenderTarget.begin(), m_mapRenderTarget.end(), CTag_Finder(wstrTargetTag));

	if (iter == m_mapRenderTarget.end())
		return nullptr;

	return iter->second;
}

list<class CRenderTarget*> * CRenderTargetMgr::Find_MultiRenderTarget(const wstring & wstrMultiTargetTag)
{
	auto	iter = find_if(m_mapMultiRenderTarget.begin(), m_mapMultiRenderTarget.end(), CTag_Finder(wstrMultiTargetTag));

	if (iter == m_mapMultiRenderTarget.end())
		return nullptr;

	return &iter->second;
}

void CRenderTargetMgr::Free()
{
	for (auto& Pair : m_mapMultiRenderTarget)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release(pRenderTarget);

		Pair.second.clear();
	}

	m_mapMultiRenderTarget.clear();

	for (auto& Pair : m_mapRenderTarget)
		Safe_Release(Pair.second);

	m_mapRenderTarget.clear();

	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
#endif // _DEBUG

}
