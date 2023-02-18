#include "stdafx.h"
#include "..\Public\Trail_Sigrid_Net.h"
#include "GameInstance.h"
#include "Sigrid.h"

CTrail_Sigrid_Net::CTrail_Sigrid_Net(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CTrail_Sigrid_Net::CTrail_Sigrid_Net(const CTrail_Sigrid_Net & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CTrail_Sigrid_Net::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTrail_Sigrid_Net::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_bActive = false;
	m_bAlpha = false;
	m_fLife = 0.15f;
	m_fWidth = 0.3f;
	m_fSegmentSize = 0.001f;
	m_vColor = FLOAT4COLOR_RGBA(53, 238, 255, 255);

	return S_OK;
}

void CTrail_Sigrid_Net::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CTrail_Sigrid_Net::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom && m_pVIBufferCom->Get_InstanceInfo()->size())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CTrail_Sigrid_Net::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTrail_Sigrid_Net::ImGui_RenderProperty()
{
	__super::ImGui_RenderProperty();
}

HRESULT CTrail_Sigrid_Net::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::SetUp_Component(), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_WhiteSRC", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	m_pVIBufferCom = CVIBuffer_PointInstancing::Create(m_pDevice, m_pContext, 300);

	return S_OK;
}

HRESULT CTrail_Sigrid_Net::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(__super::SetUp_ShaderResource(), E_FAIL);

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");

	Safe_Release(pGameInstance);

	return S_OK;
}

CTrail_Sigrid_Net * CTrail_Sigrid_Net::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTrail_Sigrid_Net*		pInstance = new CTrail_Sigrid_Net(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTrail_Sigrid_Net");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrail_Sigrid_Net::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTrail_Sigrid_Net*		pInstance = new CTrail_Sigrid_Net(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CTrail_Sigrid_Net");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Sigrid_Net::Free()
{
	__super::Free();
}