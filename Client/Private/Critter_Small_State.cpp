#include "stdafx.h"
#include "..\Public\Critter_Small_State.h"
#include "GameInstance.h"
#include "Critter_Small.h"
#include "Sigrid.h"

CCritter_Small_State::CCritter_Small_State()
{
}

HRESULT CCritter_Small_State::Initialize(CCritter_Small * pCritter, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pCritter = pCritter;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModel;
	m_pTransformCom = pTransform;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Stun(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Walk(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Sprint(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Tackle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Stagger(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Death(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Explosion(), E_FAIL);

	return S_OK;
}

void CCritter_Small_State::Tick(_double dTimeDelta)
{
}

void CCritter_Small_State::Late_Tick(_double dTimeDelta)
{
	if (m_pCritter->m_pPlayer->Collision_Body(m_pCritter->m_pRangeCol))
		m_pCritter->m_bPlayerDetected = true;
	else
		m_pCritter->m_bPlayerDetected = false;

	if (m_pCritter->m_pPlayer->Collision_Body(m_pCritter->m_pSphereCol))
		m_pCritter->m_pPlayer->Collision_Event(m_pCritter);

	if (m_pCritter->m_pPlayer->Collision_Net(m_pCritter->m_pSphereCol))
		m_pCritter->Collision_Event(m_pCritter->m_pPlayer);
}

HRESULT CCritter_Small_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE_STILL_LOOP")
		.Add_State(L"IDLE_STILL_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Idle)
		.Init_Tick(this, &CCritter_Small_State::Tick_Idle)
		.Init_End(this, &CCritter_Small_State::End_Idle)
		.Init_Changer(L"WALK_LOOP", this, &CCritter_Small_State::Check_IdleFinishCount)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Player_DetectedAndClose)
		.Init_Changer(L"SPRINT_LOOP", this, &CCritter_Small_State::Player_DetectedAndFar)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Stun()
{
	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Walk()
{
	m_pStateMachine->Add_State(L"WALK_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Walk)
		.Init_Tick(this, &CCritter_Small_State::Tick_Walk)
		.Init_End(this, &CCritter_Small_State::End_Walk)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Check_WalkTime)

		.Finish_Setting();
	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Sprint()
{
	m_pStateMachine->Add_State(L"SPRINT_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Sprint)
		.Init_Tick(this, &CCritter_Small_State::Tick_Sprint)
		.Init_End(this, &CCritter_Small_State::End_Sprint)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Player_NotDetected)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Player_DetectedAndClose)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Tackle()
{
	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Stagger()
{
	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Death()
{
	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Explosion()
{
	return S_OK;
}

void CCritter_Small_State::Start_Idle(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_STILL_LOOP);
	m_iIdleFinishCount = 0;
}

void CCritter_Small_State::Start_Walk(_double dTimeDelta)
{
	m_dWalkTime = 0.0;
	m_pModelCom->Set_CurAnimationIndex(WALK_LOOP);
}

void CCritter_Small_State::Start_Sprint(_double dTimeDelta)
{
}

void CCritter_Small_State::Tick_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationFinish())
		m_iIdleFinishCount++;

	if (m_iIdleFinishCount % 2 == 0 && rand() % 10 == 0)
	{
		_float		fCW;
		if (rand() % 3 == 0)
			fCW = 1.f;
		else
			fCW = -1.f;

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(_float(rand() % 180 + 180) * dTimeDelta * 0.4f * fCW));
	}
}

void CCritter_Small_State::Tick_Walk(_double dTimeDelta)
{
	m_dWalkTime += dTimeDelta;

	m_pTransformCom->Go_Straight(dTimeDelta, m_pCritter->m_pNavigationCom);
}

void CCritter_Small_State::Tick_Sprint(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Idle(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Walk(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Sprint(_double dTimeDelta)
{
}

_bool CCritter_Small_State::Player_Detected()
{
	return m_pCritter->m_bPlayerDetected;
}

_bool CCritter_Small_State::Player_DetectedAndFar()
{
	return _bool();
}

_bool CCritter_Small_State::Player_DetectedAndClose()
{
	return _bool();
}

_bool CCritter_Small_State::Player_NotDetected()
{
	return !m_pCritter->m_bPlayerDetected;
}

_bool CCritter_Small_State::Check_IdleFinishCount()
{
	if (m_pCritter->m_bPlayerDetected == false && m_iIdleFinishCount > 3)
	{
		if (rand() % 5 == 0)
			return true;
	}

	return false;
}

_bool CCritter_Small_State::Check_WalkTime()
{
	if (m_pCritter->m_bPlayerDetected == false && m_dWalkTime > 2.f)
	{
		if (rand() % 3 == 0)
			return true;
	}

	return false;
}

CCritter_Small_State * CCritter_Small_State::Create(CCritter_Small * pCritter, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	CCritter_Small_State*	pInstance = new CCritter_Small_State;

	if (FAILED(pInstance->Initialize(pCritter, pStateMachineCom, pModel, pTransform)))
	{
		MSG_BOX("Failed to Create : CCritter_Small_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCritter_Small_State::Free()
{
}