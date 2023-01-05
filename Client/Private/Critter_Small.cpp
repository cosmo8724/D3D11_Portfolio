#include "stdafx.h"
#include "..\Public\Critter_Small.h"
#include "GameInstance.h"
#include "Critter_Small_State.h"
#include "Sigrid.h"

CCritter_Small::CCritter_Small(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEnemy(pDevice, pContext)
{
}

CCritter_Small::CCritter_Small(const CCritter_Small & rhs)
	: CEnemy(rhs)
{
}

HRESULT CCritter_Small::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CCritter_Small::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 5.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(187.f, 8.65f, 235.f, 1.f));

	m_pCritterSmall_State = CCritter_Small_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom);

	m_pModelCom->Set_CurAnimationIndex(CCritter_Small_State::IDLE_STILL_LOOP);

	return S_OK;
}

void CCritter_Small::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	SetOn_Navigation();

	m_pCritterSmall_State->Tick(dTimeDelta);
	m_pStateMachineCom->Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);

	m_pRangeCol->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCol->Update(m_pTransformCom->Get_WorldMatrix());
}

void CCritter_Small::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pCritterSmall_State->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCritter_Small::Render()
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

#ifdef _DEBUG
	m_pRangeCol->Render();
	m_pSphereCol->Render();

	//m_pNavigationCom->Render();
#endif // _DEBUG

	return S_OK;
}

void CCritter_Small::Collision_Event(CSigrid * pPlayer)
{
}

HRESULT CCritter_Small::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Critter_Small", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("CritterSmall_01_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.5f, Ymax - Ymin, (Zmax - Zmin) * 0.9f);
	ColliderDesc.vPosition = _float3(0.f, 0.9f, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(-90.f), 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(20.f, 20.f, 20.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Range", (CComponent**)&m_pRangeCol, this, &ColliderDesc), E_FAIL);

	CNavigation::NAVIGATIONDESC		NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NavigationDesc.iCurrentIndex = 208;

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", L"Com_Navigation", (CComponent**)&m_pNavigationCom, this, &NavigationDesc), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CCritter_Small::SetUp_ShaderResource()
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

void CCritter_Small::SetOn_Navigation()
{
	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	vPos = m_pNavigationCom->Get_CellHeight(vPos);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);
}

CCritter_Small * CCritter_Small::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CCritter_Small*		pInstance = new CCritter_Small(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCritter_Small");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCritter_Small::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CCritter_Small*		pInstance = new CCritter_Small(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CCritter_Small");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCritter_Small::Free()
{
	__super::Free();

	Safe_Release(m_pCritterSmall_State);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
