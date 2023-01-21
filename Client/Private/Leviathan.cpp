#include "stdafx.h"
#include "..\Public\Leviathan.h"
#include "GameInstance.h"
#include "Leviathan_State.h"
#include "Sigrid.h"

CLeviathan::CLeviathan(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEnemy(pDevice, pContext)
{
}

CLeviathan::CLeviathan(const CLeviathan & rhs)
	: CEnemy(rhs)
{
}

HRESULT CLeviathan::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CLeviathan::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 10.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(190.f, 0.f, 767.f, 1.f));

	m_pLeviathan_State = CLeviathan_State::Create(m_pDevice, m_pContext, this, m_pStateMachineCom, m_pModelCom, m_pTransformCom);

	m_pModelCom->Set_CurAnimationIndex(0);

	m_tStatus.iMaxHP = 100;
	m_tStatus.iHP = m_tStatus.iMaxHP;
	m_tStatus.iAttack = 10;
	m_tStatus.iSpecialAttack = 20;
	m_tStatus.dInitAttackCoolTime = 3.0;
	m_tStatus.dCurAttackCoolTime = 0.0;
	m_tStatus.dInitHitCoolTime = 1.5;
	m_tStatus.dCurHitCoolTime = 0.0;

	return S_OK;
}

void CLeviathan::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	SetOn_Navigation();

	m_pLeviathan_State->Tick(dTimeDelta);
	//m_pStateMachineCom->Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);

	m_pRangeCol->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCol->Update(m_pTransformCom->Get_WorldMatrix());
}

void CLeviathan::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pLeviathan_State->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 20.f))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pRendererCom->Add_DebugRenderGroup(m_pRangeCol);
		m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
		//m_pRendererCom->Add_DebugRenderGroup(m_pNavigationCom);
	}
}

HRESULT CLeviathan::Render()
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

void CLeviathan::Collision_Event(CSigrid * pPlayer)
{
	if (pPlayer == nullptr)
		return;

	if (m_tStatus.dCurHitCoolTime < m_tStatus.dInitHitCoolTime)
		m_bHit = false;
	else
	{
		m_bHit = true;
		m_tStatus.dCurHitCoolTime = 0.0;
	}

	if (m_bHit == true)
	{
		if (m_pPlayer->Is_SpecialAttack() == false)
			m_tStatus.iHP -= pPlayer->Get_Status().iAttack;
		else
			m_tStatus.iHP -= pPlayer->Get_Status().iSpecialAttack;
	}

	if (m_tStatus.iHP < 0)
		m_tStatus.iHP = 0;
}

HRESULT CLeviathan::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Leviathan", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("SU0004_00out.md", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.5f, Ymax - Ymin, (Zmax - Zmin) * 0.9f);
	ColliderDesc.vPosition = _float3(0.f, 0.9f, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(-90.f), 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(30.f, 30.f, 30.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Range", (CComponent**)&m_pRangeCol, this, &ColliderDesc), E_FAIL);

	CNavigation::NAVIGATIONDESC		NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NavigationDesc.iCurrentIndex = 0;

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", L"Com_Navigation", (CComponent**)&m_pNavigationCom, this, &NavigationDesc), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CLeviathan::SetUp_ShaderResource()
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

void CLeviathan::SetOn_Navigation()
{
	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	vPos = m_pNavigationCom->Get_CellHeight(vPos);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);
}

CLeviathan * CLeviathan::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CLeviathan*		pInstance = new CLeviathan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CLeviathan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLeviathan::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CLeviathan*		pInstance = new CLeviathan(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CLeviathan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeviathan::Free()
{
	__super::Free();

	Safe_Release(m_pLeviathan_State);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
