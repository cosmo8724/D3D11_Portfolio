#include "stdafx.h"
#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "RenderTargetMgr.h"

CRenderer::CRenderer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
	, m_pRenderTargetMgr(CRenderTargetMgr::GetInstance())
{
	Safe_AddRef(m_pRenderTargetMgr);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	assert(eRenderGroup < RENDERGROUP_END);
	assert(pGameObject != nullptr);

	m_RenderObjectList[eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	FAILED_CHECK_RETURN(Render_Priority(), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonAlphaBlend(), E_FAIL);
	FAILED_CHECK_RETURN(Render_LightAcc(), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonLight(), E_FAIL);
	FAILED_CHECK_RETURN(Render_AlphaBlend(), E_FAIL);
	FAILED_CHECK_RETURN(Render_UI(), E_FAIL);

#ifdef _DEBUG
	if (m_pRenderTargetMgr != nullptr)
	{
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Diffuse", 100.f, 100.f, 200.f, 200.f), E_FAIL);
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Normal", 100.f, 300.f, 200.f, 200.f), E_FAIL);
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Shade", 300.f, 100.f, 200.f, 200.f), E_FAIL);

		m_pRenderTargetMgr->Render_Debug(L"MRT_Deferred");
		m_pRenderTargetMgr->Render_Debug(L"MRT_LightAcc");
	}
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	D3D11_VIEWPORT	ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint	iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Diffuse", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Normal", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Shade", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_Deferred", L"Target_Diffuse"), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_Deferred", L"Target_Normal"), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_LightAcc", L"Target_Shade"), E_FAIL);

#ifdef _DEBUG
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Diffuse", 100.f, 100.f, 200.f, 200.f), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Normal", 100.f, 300.f, 200.f, 200.f), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Shade", 300.f, 100.f, 200.f, 200.f), E_FAIL);
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	NULL_CHECK_RETURN(m_pRenderTargetMgr, E_FAIL);

	//FAILED_CHECK_RETURN(m_pRenderTargetMgr->Begin_MultiRenderTarget(m_pContext, L"MRT_Deferred"), E_FAIL);

	for (auto& pGameObject : m_RenderObjectList[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_NONALPHABLEND].clear();

	//FAILED_CHECK_RETURN(m_pRenderTargetMgr->End_MultiRenderTarget(m_pContext, L"MRT_Deferred"), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	NULL_CHECK_RETURN(m_pRenderTargetMgr, E_FAIL);

	//m_pRenderTargetMgr->Begin_MultiRenderTarget(m_pContext, L"MRT_LightAcc");
	
	//m_pRenderTargetMgr->End_MultiRenderTarget(m_pContext, L"MRT_LightAcc");

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjectList[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_UI].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(CGameObject * pOwner, void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pRenderTargetMgr);
}
