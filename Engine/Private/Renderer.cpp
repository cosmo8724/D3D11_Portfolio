#include "stdafx.h"
#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "RenderTargetMgr.h"
#include "LightMgr.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"

CRenderer::CRenderer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
	, m_pRenderTargetMgr(CRenderTargetMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
{
	Safe_AddRef(m_pRenderTargetMgr);
	Safe_AddRef(m_pLightMgr);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject, _bool bFirst)
{
	assert(eRenderGroup < RENDERGROUP_END);
	assert(pGameObject != nullptr);

	if (bFirst == false)
		m_RenderObjectList[eRenderGroup].push_back(pGameObject);
	else
		m_RenderObjectList[eRenderGroup].push_front(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugRenderGroup(CComponent * pComponent)
{
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_DebugObjectList.push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup(_bool bRenderOFF)
{
	FAILED_CHECK_RETURN(Render_Priority(bRenderOFF), E_FAIL);
	FAILED_CHECK_RETURN(Render_ShadowDepth(bRenderOFF), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonAlphaBlend(bRenderOFF), E_FAIL);
	FAILED_CHECK_RETURN(Render_LightAcc(bRenderOFF), E_FAIL);
	FAILED_CHECK_RETURN(Render_Blend(bRenderOFF), E_FAIL);
	FAILED_CHECK_RETURN(Render_NonLight(bRenderOFF), E_FAIL);
	FAILED_CHECK_RETURN(Render_AlphaBlend(bRenderOFF), E_FAIL);

#ifdef _DEBUG
	FAILED_CHECK_RETURN(Render_DebugObject(bRenderOFF), E_FAIL);
#endif // _DEBUG

#ifndef _DEBUG
	for (auto& pComponent : m_DebugObjectList)
		Safe_Release(pComponent);

	m_DebugObjectList.clear();
#endif

	FAILED_CHECK_RETURN(Render_UI(), E_FAIL);

#ifdef _DEBUG
	if (m_pRenderTargetMgr != nullptr)
	{
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Diffuse", 100.f, 100.f, 200.f, 200.f), E_FAIL);
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Normal", 100.f, 300.f, 200.f, 200.f), E_FAIL);
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Depth", 100.f, 500.f, 200.f, 200.f), E_FAIL);

		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Shade", 300.f, 100.f, 200.f, 200.f), E_FAIL);
		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Specular", 300.f, 300.f, 200.f, 200.f), E_FAIL);

		FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_ShadowDepth", 300.f, 500.f, 200.f, 200.f), E_FAIL);

		//m_pRenderTargetMgr->Render_Debug(L"MRT_Deferred");
		//m_pRenderTargetMgr->Render_Debug(L"MRT_LightAcc");
		m_pRenderTargetMgr->Render_Debug(L"MRT_LightDepth");
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

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Diffuse", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Normal", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Depth", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Shade", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_Specular", (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f)), E_FAIL);

	_uint	iDepthStencilShadowX = 16384;// 10240;
	_uint	iDepthStencilShadowY = 9472;// 5920;

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_RenderTarget(m_pDevice, m_pContext, L"Target_ShadowDepth", iDepthStencilShadowX, iDepthStencilShadowY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f)), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_Deferred", L"Target_Diffuse"), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_Deferred", L"Target_Normal"), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_Deferred", L"Target_Depth"), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_LightAcc", L"Target_Shade"), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_LightAcc", L"Target_Specular"), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Add_MultiRenderTarget(L"MRT_LightDepth", L"Target_ShadowDepth"), E_FAIL);

	m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_Deferred.hlsl", CShader::DECLARATION_VTXTEX, VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	XMStoreFloat4x4(&m_matWorld, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Diffuse", 100.f, 100.f, 200.f, 200.f), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Normal", 100.f, 300.f, 200.f, 200.f), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Depth", 100.f, 500.f, 200.f, 200.f), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Shade", 300.f, 100.f, 200.f, 200.f), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_Specular", 300.f, 300.f, 200.f, 200.f), E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Ready_Debug(L"Target_ShadowDepth", 300.f, 500.f, 200.f, 200.f), E_FAIL);
#endif // _DEBUG

	return S_OK;
}

HRESULT CRenderer::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

void CRenderer::ImGui_RenderProperty()
{
	if (ImGui::Button("Recompile Shader"))
		m_pShaderCom->ReCompile();
}

HRESULT CRenderer::Render_Priority(_bool bRenderOFF)
{
	if (bRenderOFF == true)
	{
		for (auto& pGameObject : m_RenderObjectList[RENDER_PRIORITY])
			Safe_Release(pGameObject);

		m_RenderObjectList[RENDER_PRIORITY].clear();

		return S_OK;
	}

	for (auto& pGameObject : m_RenderObjectList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth(_bool bRenderOFF)
{
	NULL_CHECK_RETURN(m_pRenderTargetMgr, E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Begin_ShadowDepthRenderTarget(m_pContext, L"Target_ShadowDepth"), E_FAIL);

	//m_pContext->ClearDepthStencilView(m_pDepthStencilViewShadow, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	/*if (bRenderOFF == true)
	{
		for (auto& pGameObject : m_RenderObjectList[RENDER_SHADOWDEPTH])
			Safe_Release(pGameObject);

		m_RenderObjectList[RENDER_SHADOWDEPTH].clear();

		return S_OK;
	}*/

	for (auto& pGameObject : m_RenderObjectList[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_ShadowDepth();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_SHADOWDEPTH].clear();

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->End_MultiRenderTarget(m_pContext, L"MRT_LightDepth"), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend(_bool bRenderOFF)
{
	if (bRenderOFF == true)
	{
		for (auto& pGameObject : m_RenderObjectList[RENDER_NONALPHABLEND])
			Safe_Release(pGameObject);

		m_RenderObjectList[RENDER_NONALPHABLEND].clear();

		return S_OK;
	}

	NULL_CHECK_RETURN(m_pRenderTargetMgr, E_FAIL);

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Begin_MultiRenderTarget(m_pContext, L"MRT_Deferred"), E_FAIL);

	for (auto& pGameObject : m_RenderObjectList[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_NONALPHABLEND].clear();

	FAILED_CHECK_RETURN(m_pRenderTargetMgr->End_MultiRenderTarget(m_pContext, L"MRT_Deferred"), E_FAIL);

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc(_bool bRenderOFF)
{
	if (bRenderOFF == true)
		return S_OK;

	NULL_CHECK_RETURN(m_pRenderTargetMgr, E_FAIL);

	D3D11_VIEWPORT	ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint	iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_matWorld, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pRenderTargetMgr->Begin_MultiRenderTarget(m_pContext, L"MRT_LightAcc");
	
	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorld);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pShaderCom->Set_Matrix(L"g_matViewInv", &CPipeLine::GetInstance()->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProjInv", &CPipeLine::GetInstance()->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ));

	m_pShaderCom->Set_RawValue(L"g_vCamPosition", &CPipeLine::GetInstance()->Get_CameraPosition(), sizeof(_float4));

	m_pShaderCom->Set_ShaderResourceView(L"g_NormalTexture", m_pRenderTargetMgr->Get_ShaderResourceView(L"Target_Normal"));
	m_pShaderCom->Set_ShaderResourceView(L"g_DepthTexture", m_pRenderTargetMgr->Get_ShaderResourceView(L"Target_Depth"));

	m_pLightMgr->Render_Light(m_pVIBufferCom, m_pShaderCom);

	m_pRenderTargetMgr->End_MultiRenderTarget(m_pContext, L"MRT_LightAcc");

	return S_OK;
}

HRESULT CRenderer::Render_Blend(_bool bRenderOFF)
{
	if (bRenderOFF == true)
		return S_OK;

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorld);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pShaderCom->Set_Matrix(L"g_matViewInv", &CPipeLine::GetInstance()->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProjInv", &CPipeLine::GetInstance()->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ));

	const LIGHTDESC*	pLightDesc = CLightMgr::GetInstance()->Get_LightDesc(0);

	if (pLightDesc != nullptr)
	{
		//_float4	vLightEye = pLightDesc->vPosition;
		//_float4	vLightAt = XMLoadFloat4(&vLightEye) + XMVector3Normalize(XMLoadFloat4(&pLightDesc->vDirection));
		//_float4	vLightUp = { 0.f, 1.f, 0.f, 0.f };

		_float4		vLightEye = _float4(0.f, 300.f, 0.f, 1.f);
		_float4		vLightAt = _float4(900.f, 0.f, 900.f, 1.f);
		_float4		vLightUp = _float4(0.f, 1.f, 0.f, 0.f);

		_float4x4	matLightView = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);

		D3D11_VIEWPORT	ViewportDesc;
		ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

		_uint	iNumViewport = 1;

		m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);

		_float4x4	matLightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)ViewportDesc.Width / (_float)ViewportDesc.Height, 0.1f, 1000.f);

		m_pShaderCom->Set_Matrix(L"g_matLightView", &matLightView);
		m_pShaderCom->Set_Matrix(L"g_matLightProj", &matLightProj);
	}

	m_pShaderCom->Set_ShaderResourceView(L"g_DiffuseTexture", m_pRenderTargetMgr->Get_ShaderResourceView(L"Target_Diffuse"));
	m_pShaderCom->Set_ShaderResourceView(L"g_ShadeTexture", m_pRenderTargetMgr->Get_ShaderResourceView(L"Target_Shade"));
	m_pShaderCom->Set_ShaderResourceView(L"g_SpecularTexture", m_pRenderTargetMgr->Get_ShaderResourceView(L"Target_Specular"));
	m_pShaderCom->Set_ShaderResourceView(L"g_ShadowDepthTexture", m_pRenderTargetMgr->Get_ShaderResourceView(L"Target_ShadowDepth"));

	m_pShaderCom->Begin(3);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CRenderer::Render_DebugObject(_bool bRenderOFF)
{
	if (bRenderOFF == true)
	{
		for (auto& pComponent : m_DebugObjectList)
			Safe_Release(pComponent);

		m_DebugObjectList.clear();

		return S_OK;
	}

	for (auto& pComponent : m_DebugObjectList)
	{
		if (nullptr != pComponent)
			pComponent->Render();

		Safe_Release(pComponent);
	}

	m_DebugObjectList.clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight(_bool bRenderOFF)
{
	if (bRenderOFF == true)
	{
		for (auto& pGameObject : m_RenderObjectList[RENDER_NONLIGHT])
			Safe_Release(pGameObject);

		m_RenderObjectList[RENDER_NONLIGHT].clear();

		return S_OK;
	}

	for (auto& pGameObject : m_RenderObjectList[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjectList[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend(_bool bRenderOFF)
{
	if (bRenderOFF == true)
	{
		for (auto& pGameObject : m_RenderObjectList[RENDER_ALPHABLEND])
			Safe_Release(pGameObject);

		m_RenderObjectList[RENDER_ALPHABLEND].clear();

		return S_OK;
	}

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

	for (_uint i = 0; i < RENDERGROUP_END; ++i)
	{
		for (auto& pGameObject : m_RenderObjectList[i])
			Safe_Release(pGameObject);
		m_RenderObjectList[i].clear();
	}

	for (auto& pComponent : m_DebugObjectList)
		Safe_Release(pComponent);
	m_DebugObjectList.clear();

	Safe_Release(m_pRenderTargetMgr);
	Safe_Release(m_pLightMgr);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
