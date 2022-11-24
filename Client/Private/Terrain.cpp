#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);
	
	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CTerrain::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CTerrain::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTerrain::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTerrain::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Terrain", L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_VIBuffer_Terrain", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Terrain", L"Com_Texture_Terrain", (CComponent**)&m_pTextureCom[TEXTURE_DIFFUSE]), E_FAIL);
	
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Brush", L"Com_Texture_Brush", (CComponent**)&m_pTextureCom[TEXTURE_BRUSH]), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Filter", L"Com_Texture_Filter", (CComponent**)&m_pTextureCom[TEXTURE_FILTER]), E_FAIL);

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, L"g_DiffuseTexture");
	m_pTextureCom[TEXTURE_BRUSH]->Bind_ShaderResource(m_pShaderCom, L"g_BrushTexture");
	m_pTextureCom[TEXTURE_FILTER]->Bind_ShaderResource(m_pShaderCom, L"g_FilterTexture");
	m_pShaderCom->Set_RawValue(L"g_vBrushPos", &_float4(15.f, 0.f, 15.f, 1.f), sizeof(_float4));

	const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	NULL_CHECK_RETURN(pLightDesc, E_FAIL);

	m_pShaderCom->Set_RawValue(L"g_vLightDir", &pLightDesc->vDirection, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4));
	m_pShaderCom->Set_RawValue(L"g_vCamPosition", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

	Safe_Release(pGameInstance);

	return S_OK;
}

CTerrain * CTerrain::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Createe : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
