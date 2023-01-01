#include "stdafx.h"
#include "..\Public\Ocean.h"
#include "GameInstance.h"
#include "GameUtility.h"

COcean::COcean(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

COcean::COcean(const COcean & rhs)
	: CGameObject(rhs)
{
}

HRESULT COcean::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT COcean::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void COcean::Tick(_double dTimeDelta)
{
	m_dTimeDelta += dTimeDelta;

	//m_pVIBufferCom->Tick(dTimeDelta);

	__super::Tick(dTimeDelta);
}

void COcean::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT COcean::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

pair<_bool, _float3> COcean::Picking_Terrain()
{
	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	_uint	iNumViewport = 1;

	m_pContext->RSGetViewports(&iNumViewport, &ViewPort);

	_float3	vPickingPoint = { 0.f, 0.f, 0.f };

	pair<_bool, _float>	PickInfo = CGameUtility::Picking(g_hWnd, ViewPort.Width, ViewPort.Height, m_pTransformCom, m_pVIBufferCom->Get_TerrainPosition(), m_pVIBufferCom->Get_NumVerticesX(), m_pVIBufferCom->Get_NumVerticesZ(), vPickingPoint);

	return pair<_bool, _float3>{ PickInfo.first, vPickingPoint };
}

HRESULT COcean::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Shader_Ocean", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Terrain", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Foam", L"Com_Texture_Foam", (CComponent**)&m_pFoamTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Height", L"Com_Texture_Height", (CComponent**)&m_pHeightTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT COcean::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	_float4x4		matViewInverse = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	_float4x4		matProjInverse = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);

	m_pShaderCom->Set_Matrix(L"g_matViewInverse", &matViewInverse);
	m_pShaderCom->Set_Matrix(L"g_matProjInverse", &matProjInverse);

	m_pTextureCom->Bind_ShaderResources(m_pShaderCom, L"g_DiffuseTexture");
	m_pFoamTextureCom->Bind_ShaderResources(m_pShaderCom, L"g_BrushTexture");
	m_pHeightTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_HeightTexture");

	const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	NULL_CHECK_RETURN(pLightDesc, E_FAIL);

	m_pShaderCom->Set_RawValue(L"g_vLightDir", &pLightDesc->vDirection, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vCamPosition", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

	_float		fTimeDelta = (_float)m_dTimeDelta;
	_float		fWaveHeight = 0.7f;
	_float		fSpeed = 3.5f;
	_float		fWaveFrequency = 100.f;
	_float		fUVSpeed = 0.02f;

	m_pShaderCom->Set_RawValue(L"g_Time", &fTimeDelta, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_WaveHeight", &fWaveHeight, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_Speed", &fSpeed, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_WaveFrequency", &fWaveFrequency, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_UVSpeed", &fUVSpeed, sizeof(_float));

	Safe_Release(pGameInstance);

	return S_OK;
}

COcean * COcean::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	COcean*		pInstance = new COcean(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Createe : COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COcean::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	COcean*		pInstance = new COcean(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COcean::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pFoamTextureCom);
	Safe_Release(m_pHeightTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
