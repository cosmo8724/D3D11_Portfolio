#include "stdafx.h"
#include "..\Public\Hollow_Knight.h"
#include "GameInstance.h"
#include "Sigrid.h"
#include "Talk_NPC_Background.h"

CHollow_Knight::CHollow_Knight(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CNPC(pDevice, pContext)
{
}

CHollow_Knight::CHollow_Knight(const CHollow_Knight & rhs)
	: CNPC(rhs)
{
}

HRESULT CHollow_Knight::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CHollow_Knight::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(575.f, 2.422f, 328.4f, 1.f));

	m_pModelCom->Set_CurAnimationIndex(0);

	m_vecSentence.push_back(L" ");
	m_vecSentence.push_back(L"..");
	m_vecSentence.push_back(L"...");
	m_vecSentence.push_back(L"......");

	return S_OK;
}

void CHollow_Knight::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);

	if (m_bPlayerDetected == true)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_E) && m_bNowTalking == false)
		{
			m_bNowTalking = true;
			CTalk_NPC_Background*	pUI = dynamic_cast<CTalk_NPC_Background*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_UI_Talk_NPC_BackGround"));
			pUI->Set_Owner(this);
			pUI->Set_Name(L"²®µ¥±â");
			pUI->Set_Sentence(&m_vecSentence);
		}
	}

	m_pRangeCol->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCol->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHollow_Knight::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pRendererCom->Add_DebugRenderGroup(m_pRangeCol);
		m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
	}
}

HRESULT CHollow_Knight::Render()
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

void CHollow_Knight::Collision_Event(CSigrid * pPlayer)
{
	if (pPlayer == nullptr)
		return;
}

HRESULT CHollow_Knight::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Hollow_Knight", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("Mesh.005", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.5f, Ymax - Ymin, (Zmax - Zmin) * 0.9f);
	ColliderDesc.vPosition = _float3(0.f, 0.9f, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(-90.f), 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Range", (CComponent**)&m_pRangeCol, this, &ColliderDesc), E_FAIL);

	return S_OK;
}

HRESULT CHollow_Knight::SetUp_ShaderResource()
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

CHollow_Knight * CHollow_Knight::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CHollow_Knight*		pInstance = new CHollow_Knight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHollow_Knight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHollow_Knight::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CHollow_Knight*		pInstance = new CHollow_Knight(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CHollow_Knight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHollow_Knight::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCol);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
