#include "stdafx.h"
#include "..\Public\Grandma_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Grandma.h"
#include "Sigrid.h"

CGrandma_State::CGrandma_State()
{
}

HRESULT CGrandma_State::Initialize(CGrandma * pGrandma, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pGrandma = pGrandma;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModelCom;
	m_pTransformCom = pTransformCom;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Surprised(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Talk(), E_FAIL);

	return S_OK;
}

void CGrandma_State::Tick(_double dTimeDelta)
{
}

void CGrandma_State::Late_Tick(_double dTImeDelta)
{
	if (m_pGrandma->m_pPlayer->Collision_Body(m_pGrandma->m_pRangeCol))
		m_pGrandma->m_bPlayerDetected = true;
	else
		m_pGrandma->m_bPlayerDetected = false;
}

HRESULT CGrandma_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE_STILL_LOOP")
.Add_State(L"IDLE_STILL_LOOP")
.Init_Start(this, &CGrandma_State::Start_Idle_Still_Loop)
.Init_Tick(this, &CGrandma_State::Tick_Idle_Still_Loop)
.Init_End(this, &CGrandma_State::End_Idle_Still_Loop)
.Init_Changer(L"SURPRISED", this, &CGrandma_State::Surprised)
.Init_Changer(L"TALK_BASE_LOOP", this, &CGrandma_State::Talking)

.Finish_Setting();

	return S_OK;
}

HRESULT CGrandma_State::SetUp_State_Surprised()
{
	m_pStateMachine->Add_State(L"SURPRISED")
		.Init_Start(this, &CGrandma_State::Start_Surprised)
		.Init_Tick(this, &CGrandma_State::Tick_Surprised)
		.Init_End(this, &CGrandma_State::End_Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CGrandma_State::Talking)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CGrandma_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CGrandma_State::SetUp_State_Talk()
{
	m_pStateMachine->Add_State(L"TALK_BASE_LOOP")
		.Init_Start(this, &CGrandma_State::Start_Talk)
		.Init_Tick(this, &CGrandma_State::Tick_Talk)
		.Init_End(this, &CGrandma_State::End_Talk)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CGrandma_State::NotTalking)

		.Finish_Setting();

	return S_OK;
}

void CGrandma_State::Start_Idle_Still_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_STILL_LOOP);
}

void CGrandma_State::Start_Surprised(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(SURPRISED);
}

void CGrandma_State::Start_Talk(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TALK_BASE_LOOP);
}

void CGrandma_State::Tick_Idle_Still_Loop(_double dTimeDelta)
{
}

void CGrandma_State::Tick_Surprised(_double dTimeDelta)
{
}

void CGrandma_State::Tick_Talk(_double dTimeDelta)
{
}

void CGrandma_State::End_Idle_Still_Loop(_double dTimeDelta)
{
}

void CGrandma_State::End_Surprised(_double dTimeDelta)
{
}

void CGrandma_State::End_Talk(_double dTimeDelta)
{
}

_bool CGrandma_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CGrandma_State::Surprised()
{
	if (m_pGrandma->m_bPlayerDetected == true && m_bSurprised == false)
	{
		m_bSurprised = true;
		return true;
	}

	return false;
}

_bool CGrandma_State::Talking()
{
	return m_pGrandma->m_bNowTalking;
}

_bool CGrandma_State::NotTalking()
{
	return !m_pGrandma->m_bNowTalking;
}

CGrandma_State * CGrandma_State::Create(CGrandma * pGrandma, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	CGrandma_State*	pInstance = new CGrandma_State;

	if (FAILED(pInstance->Initialize(pGrandma, pStateMachineCom, pModelCom, pTransformCom)))
	{
		MSG_BOX("Failed to Create : CGrandma_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrandma_State::Free()
{
}


