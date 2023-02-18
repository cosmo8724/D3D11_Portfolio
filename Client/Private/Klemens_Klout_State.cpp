#include "stdafx.h"
#include "..\Public\Klemens_Klout_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Klemens_Klout.h"
#include "Sigrid.h"

CKlemens_Klout_State::CKlemens_Klout_State()
{
}

HRESULT CKlemens_Klout_State::Initialize(CKlemens_Klout * pKlout, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pKlout = pKlout;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModelCom;
	m_pTransformCom = pTransformCom;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Surprised(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Talk(), E_FAIL);

	return S_OK;
}

void CKlemens_Klout_State::Tick(_double dTimeDelta)
{
}

void CKlemens_Klout_State::Late_Tick(_double dTImeDelta)
{
	if (m_pKlout->m_pPlayer->Collision_Body(m_pKlout->m_pRangeCol))
	{
		m_pKlout->m_bPlayerDetected = true;
	}
	else
	{
		m_pKlout->m_bPlayerDetected = false;
		m_bSurprised = false;
	}
}

HRESULT CKlemens_Klout_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE_HAPPY_LOOP")
		.Add_State(L"IDLE_HAPPY_LOOP")
		.Init_Start(this, &CKlemens_Klout_State::Start_Idle_Happy_Loop)
		.Init_Tick(this, &CKlemens_Klout_State::Tick_Idle_Happy_Loop)
		.Init_End(this, &CKlemens_Klout_State::End_Idle_Happy_Loop)
		.Init_Changer(L"SURPRISED", this, &CKlemens_Klout_State::Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CKlemens_Klout_State::Talking)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKlemens_Klout_State::SetUp_State_Surprised()
{
	m_pStateMachine->Add_State(L"SURPRISED")
		.Init_Start(this, &CKlemens_Klout_State::Start_Surprised)
		.Init_Tick(this, &CKlemens_Klout_State::Tick_Surprised)
		.Init_End(this, &CKlemens_Klout_State::End_Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CKlemens_Klout_State::Talking)
		.Init_Changer(L"IDLE_HAPPY_LOOP", this, &CKlemens_Klout_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CKlemens_Klout_State::SetUp_State_Talk()
{
	m_pStateMachine->Add_State(L"TALK_BASE_LOOP")
		.Init_Start(this, &CKlemens_Klout_State::Start_Talk)
		.Init_Tick(this, &CKlemens_Klout_State::Tick_Talk)
		.Init_End(this, &CKlemens_Klout_State::End_Talk)
		.Init_Changer(L"IDLE_HAPPY_LOOP", this, &CKlemens_Klout_State::NotTalking)

		.Finish_Setting();

	return S_OK;
}

void CKlemens_Klout_State::Start_Idle_Happy_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_HAPPY_LOOP);
}

void CKlemens_Klout_State::Start_Surprised(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(SURPRISED);
}

void CKlemens_Klout_State::Start_Talk(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TALK_BASE_LOOP);
}

void CKlemens_Klout_State::Tick_Idle_Happy_Loop(_double dTimeDelta)
{
}

void CKlemens_Klout_State::Tick_Surprised(_double dTimeDelta)
{
}

void CKlemens_Klout_State::Tick_Talk(_double dTimeDelta)
{
}

void CKlemens_Klout_State::End_Idle_Happy_Loop(_double dTimeDelta)
{
}

void CKlemens_Klout_State::End_Surprised(_double dTimeDelta)
{
}

void CKlemens_Klout_State::End_Talk(_double dTimeDelta)
{
}

_bool CKlemens_Klout_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CKlemens_Klout_State::Surprised()
{
	if (m_pKlout->m_bPlayerDetected == true && m_bSurprised == false)
	{
		m_bSurprised = true;
		return true;
	}

	return false;
}

_bool CKlemens_Klout_State::Talking()
{
	return m_pKlout->m_bNowTalking;
}

_bool CKlemens_Klout_State::NotTalking()
{
	return !m_pKlout->m_bNowTalking;
}

CKlemens_Klout_State * CKlemens_Klout_State::Create(CKlemens_Klout * pKlout, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	CKlemens_Klout_State*	pInstance = new CKlemens_Klout_State;

	if (FAILED(pInstance->Initialize(pKlout, pStateMachineCom, pModelCom, pTransformCom)))
	{
		MSG_BOX("Failed to Create : CKlemens_Klout_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKlemens_Klout_State::Free()
{
}