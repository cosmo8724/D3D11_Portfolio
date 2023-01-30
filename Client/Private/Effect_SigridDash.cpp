#include "stdafx.h"
#include "..\Public\Effect_SigridDash.h"
#include "GameInstance.h"

CEffect_SigridDash::CEffect_SigridDash(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CEffect_SigridDash::CEffect_SigridDash(const CEffect_SigridDash & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CEffect_SigridDash::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_SigridDash::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CEffect_SigridDash::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bScaleFix == false)
	{
		m_pTransformCom->Set_Scale(_float3(0.01f, 0.01f, 0.01f));
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_bScaleFix = true;
	}

	m_dLimitTime += dTimeDelta;
	m_fFadeAlpha -= (_float)dTimeDelta * 0.5f;
}

void CEffect_SigridDash::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_fFadeAlpha < 0.f)
	{
		m_bDead = true;
		return;
	}

	if (nullptr != m_pRendererCom && m_bScaleFix == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_SigridDash::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Render(m_pShaderCom, 0, L"", 2);

	return S_OK;
}

void CEffect_SigridDash::ImGui_RenderProperty()
{
}

HRESULT CEffect_SigridDash::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_WhiteSRC", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Sigrid_Dash", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CEffect_SigridDash::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
	m_pShaderCom->Set_RawValue(L"g_fFadeAlpha", &m_fFadeAlpha, sizeof(_float));

	Safe_Release(pGameInstance);

	return S_OK;
}

CEffect_SigridDash * CEffect_SigridDash::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CEffect_SigridDash*		pInstance = new CEffect_SigridDash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_SigridDash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_SigridDash::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CEffect_SigridDash*		pInstance = new CEffect_SigridDash(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_SigridDash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_SigridDash::Free()
{
	__super::Free();
}
