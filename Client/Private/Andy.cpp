#include "stdafx.h"
#include "..\Public\Andy.h"
#include "GameInstance.h"
#include "Andy_State.h"
#include "Talk_NPC_Background.h"
#include "GameUtility.h"
#include "Sigrid.h"
#include "Leviathan.h"

CAndy::CAndy(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CNPC(pDevice, pContext)
{
}

CAndy::CAndy(const CAndy & rhs)
	: CNPC(rhs)
{
}

HRESULT CAndy::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CAndy::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(55.584f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(375.342f, 2.858f, 778.915f, 1.f));

	m_pAndy_State = CAndy_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom);

	m_pModelCom->Set_CurAnimationIndex(CAndy_State::COWERING_LOOP);

	m_vecSentence.push_back(L"");
	m_vecSentence.push_back(L"이... 이봐! 누군지는 모르겠지만 일단 멈춰!!!");
	m_vecSentence.push_back(L"이 앞은 어차피 막다른 길이야.");
	m_vecSentence.push_back(L"게다가 얼마 전부터 처음보는 몬스터가 이 곳에 자리를 잡았어!");
	m_vecSentence.push_back(L"포탈을 찾고 있는 것이라면 포기하는게 좋을거야...");

	return S_OK;
}

void CAndy::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pAndy_State->Tick(dTimeDelta);
	m_pStateMachineCom->Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta);

	if (m_bQuest == true && m_bNowTalking == false && m_pPlayer->Get_IsBossBattle() == false)
	{
		if (m_bQuestDone == false)
		{
			while (m_vecSentence.size() > 1)
				m_vecSentence.pop_back();

			m_vecSentence.push_back(L"조심하는게 좋을거야...");

			if (dynamic_cast<CLeviathan*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Enemies")->front())->Get_Die() == true)
				m_bQuestDone = true;
		}
		else
		{
			while (m_vecSentence.size() > 1)
				m_vecSentence.pop_back();

			m_vecSentence.push_back(L"허거덩~ 너 정말 강하구나!\n약속대로 뒤에 있는 대포를 작동시켜줄게.");
		}
	}

	if (m_bPlayerDetected == true)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_E) && m_bNowTalking == false)
		{
			m_bNowTalking = true;
			CTalk_NPC_Background*	pUI = dynamic_cast<CTalk_NPC_Background*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_UI_Talk_NPC_BackGround"));
			pUI->Set_Owner(this);
			pUI->Set_Name(L"앤디");
			pUI->Set_Sentence(&m_vecSentence);
		}
	}

	m_pRangeCol->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCol->Update(m_pTransformCom->Get_WorldMatrix());
}

void CAndy::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pAndy_State->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		m_pRendererCom->Add_DebugRenderGroup(m_pRangeCol);
		m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
	}
}

HRESULT CAndy::Render()
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

void CAndy::Collision_Event(CSigrid * pPlayer)
{
	if (pPlayer == nullptr)
		return;
}

void CAndy::Play_Voice()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"Andy_Emotes_Scared_010_EN.wav", g_fVoiceVolume, false, true, 17);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"Andy_Emotes_Scared_020_EN.wav", g_fVoiceVolume, false, true, 17);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"Andy_Emotes_Scared_030_EN.wav", g_fVoiceVolume, false, true, 17);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"Andy_Emotes_Scared_040_EN.wav", g_fVoiceVolume, false, true, 17);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"Andy_Emotes_Scared_050_EN.wav", g_fVoiceVolume, false, true, 17);
}

HRESULT CAndy::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Andy", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("Andy_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

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

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CAndy::SetUp_ShaderResource()
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

CAndy * CAndy::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CAndy*		pInstance = new CAndy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CAndy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAndy::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CAndy*		pInstance = new CAndy(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CAndy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAndy::Free()
{
	__super::Free();

	Safe_Release(m_pAndy_State);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
