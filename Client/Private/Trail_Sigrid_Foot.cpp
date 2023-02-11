#include "stdafx.h"
#include "..\Public\Trail_Sigrid_Foot.h"
#include "GameInstance.h"
#include "Sigrid.h"

CTrail_Sigrid_Foot::CTrail_Sigrid_Foot(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Trail(pDevice, pContext)
{
}

CTrail_Sigrid_Foot::CTrail_Sigrid_Foot(const CTrail_Sigrid_Foot & rhs)
	: CEffect_Trail(rhs)
{
}

HRESULT CTrail_Sigrid_Foot::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTrail_Sigrid_Foot::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_bActive = true;
	m_bAlpha = true;
	m_fLife = 0.3f;
	m_fWidth = 0.1f;
	m_fSegmentSize = 0.001f;

	return S_OK;
}

void CTrail_Sigrid_Foot::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CTrail_Sigrid_Foot::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom && m_pVIBufferCom->Get_InstanceInfo()->size())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CTrail_Sigrid_Foot::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTrail_Sigrid_Foot::ImGui_RenderProperty()
{
	__super::ImGui_RenderProperty();
}

HRESULT CTrail_Sigrid_Foot::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::SetUp_Component(), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_WhiteSRC", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	//FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_VIBuffer_Trail", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);

	m_pVIBufferCom = CVIBuffer_PointInstancing::Create(m_pDevice, m_pContext, 300);

	return S_OK;
}

HRESULT CTrail_Sigrid_Foot::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(__super::SetUp_ShaderResource(), E_FAIL);

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");

	Safe_Release(pGameInstance);

	return S_OK;
}

CTrail_Sigrid_Foot * CTrail_Sigrid_Foot::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTrail_Sigrid_Foot*		pInstance = new CTrail_Sigrid_Foot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTrail_Sigrid_Foot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrail_Sigrid_Foot::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTrail_Sigrid_Foot*		pInstance = new CTrail_Sigrid_Foot(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CTrail_Sigrid_Foot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Sigrid_Foot::Free()
{
	__super::Free();
}
