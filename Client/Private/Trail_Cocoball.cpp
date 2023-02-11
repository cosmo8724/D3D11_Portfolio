#include "stdafx.h"
#include "..\Public\Trail_Cocoball.h"
#include "GameInstance.h"
#include "Effect_Cocoball.h"

CTrail_Cocoball::CTrail_Cocoball(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CTrail_Cocoball::CTrail_Cocoball(const CTrail_Cocoball & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CTrail_Cocoball::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTrail_Cocoball::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_bActive = false;
	m_bAlpha = false;
	m_fLife = 0.8f;
	m_fWidth = 0.5f;
	m_fSegmentSize = 0.001f;
	m_vColor = { 1.f, 0.5f, 0.2f, 1.f };

	return S_OK;
}

void CTrail_Cocoball::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CTrail_Cocoball::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom && m_pVIBufferCom->Get_InstanceInfo()->size())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTrail_Cocoball::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTrail_Cocoball::ImGui_RenderProperty()
{
	__super::ImGui_RenderProperty();
}

HRESULT CTrail_Cocoball::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::SetUp_Component(), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_WhiteSRC", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	m_pVIBufferCom = CVIBuffer_PointInstancing::Create(m_pDevice, m_pContext, 300);

	return S_OK;
}

HRESULT CTrail_Cocoball::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(__super::SetUp_ShaderResource(), E_FAIL);

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");

	Safe_Release(pGameInstance);

	return S_OK;
}

CTrail_Cocoball * CTrail_Cocoball::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTrail_Cocoball*		pInstance = new CTrail_Cocoball(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTrail_Cocoball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrail_Cocoball::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTrail_Cocoball*		pInstance = new CTrail_Cocoball(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CTrail_Cocoball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Cocoball::Free()
{
	__super::Free();
}
