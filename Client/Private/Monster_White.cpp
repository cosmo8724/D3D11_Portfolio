#include "stdafx.h"
#include "..\Public\Monster_White.h"
#include "GameInstance.h"
#include "Sigrid.h"
#include "MonsterDrink_Frame.h"

CMonster_White::CMonster_White(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CMonsterDrink(pDevice, pContext)
{
}

CMonster_White::CMonster_White(const CMonster_White & rhs)
	: CMonsterDrink(rhs)
{
}

HRESULT CMonster_White::Initialize_Prototype()
{
	m_bHasModel = true;
	m_iValue = 50;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CMonster_White::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 15.5;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(45.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CMonster_White::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_pUIFrame == nullptr)
	{
		list<CGameObject*>*		pUIList = CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_UI");

		for (auto& pGameObject : *pUIList)
		{
			m_pUIFrame = dynamic_cast<CMonsterDrink_Frame*>(pGameObject);

			if (m_pUIFrame != nullptr)
			{
				if (m_pUIFrame->Get_UITag() == L"UI_MonsterDrink_White")
					break;
			}
		}
	}
}

void CMonster_White::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_pPlayer->Collision_Body(m_pSphereCol))
	{
		m_pPlayer->Get_Status().iMonsterDrink_White++;
		m_bDead = true;
		m_pUIFrame->Set_ValueChange();
	}

	if (m_bDead == true)
		return;

	if (nullptr != m_pRendererCom)
	{
		if (true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
	}
}

HRESULT CMonster_White::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CMonster_White::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Monster_White", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CMonster_White::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster_White * CMonster_White::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CMonster_White*		pInstance = new CMonster_White(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CMonster_White");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonster_White::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CMonster_White*		pInstance = new CMonster_White(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CMonster_White");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_White::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
