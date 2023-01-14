#include "stdafx.h"
#include "..\Public\Shinobi.h"
#include "GameInstance.h"

CShinobi::CShinobi(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEnemy(pDevice, pContext)
{
}

CShinobi::CShinobi(const CShinobi & rhs)
	: CEnemy(rhs)
{
}

HRESULT CShinobi::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShinobi::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 3.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(187.f, 8.65f, 235.f, 1.f));

	//m_pCritterSmall_State = CCritter_Small_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom);

	m_pModelCom->Set_CurAnimationIndex(0);

	m_tStatus.iMaxHP = 20;
	m_tStatus.iHP = m_tStatus.iMaxHP;
	m_tStatus.iAttack = 5;
	m_tStatus.iSpecialAttack = 10;
	m_tStatus.dInitAttackCoolTime = 3.0;
	m_tStatus.dCurAttackCoolTime = 0.0;
	m_tStatus.dInitHitCoolTime = 1.5;
	m_tStatus.dCurHitCoolTime = 0.0;

	return S_OK;
}

void CShinobi::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);
}

void CShinobi::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CShinobi::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones");
	}

	return S_OK;
}

void CShinobi::Collision_Event(CSigrid * pPlayer)
{
}

HRESULT CShinobi::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Shinobi", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShinobi::SetUp_ShaderResource()
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

void CShinobi::SetOn_Navigation()
{
}

CShinobi * CShinobi::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShinobi*		pInstance = new CShinobi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShinobi");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShinobi::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShinobi*		pInstance = new CShinobi(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CShinobi");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShinobi::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
