#include "stdafx.h"
#include "..\Public\Andy_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Andy.h"
#include "Sigrid.h"

CAndy_State::CAndy_State()
{
}

HRESULT CAndy_State::Initialize(CAndy * pAndy, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pAndy = pAndy;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModelCom;
	m_pTransformCom = pTransformCom;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Surprised(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Talk(), E_FAIL);

	return S_OK;
}

void CAndy_State::Tick(_double dTimeDelta)
{
}

void CAndy_State::Late_Tick(_double dTImeDelta)
{
	if (m_pAndy->m_pPlayer->Collision_Body(m_pAndy->m_pRangeCol))
	{
		m_pAndy->m_bPlayerDetected = true;
	}
	else
	{
		m_pAndy->m_bPlayerDetected = false;
		m_bSurprised = false;
	}
}

HRESULT CAndy_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"COWERING_LOOP")
		.Add_State(L"COWERING_LOOP")
		.Init_Start(this, &CAndy_State::Start_Cowering)
		.Init_Tick(this, &CAndy_State::Tick_Cowering)
		.Init_End(this, &CAndy_State::End_Cowering)
		.Init_Changer(L"SURPRISED", this, &CAndy_State::Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CAndy_State::Talking)

		.Finish_Setting();

	return S_OK;
}

HRESULT CAndy_State::SetUp_State_Surprised()
{
	m_pStateMachine->Add_State(L"SURPRISED")
		.Init_Start(this, &CAndy_State::Start_Surprised)
		.Init_Tick(this, &CAndy_State::Tick_Surprised)
		.Init_End(this, &CAndy_State::End_Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CAndy_State::Talking)
		.Init_Changer(L"COWERING_LOOP", this, &CAndy_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CAndy_State::SetUp_State_Talk()
{
	m_pStateMachine->Add_State(L"TALK_BASE_LOOP")
		.Init_Start(this, &CAndy_State::Start_Talk)
		.Init_Tick(this, &CAndy_State::Tick_Talk)
		.Init_End(this, &CAndy_State::End_Talk)
		.Init_Changer(L"COWERING_LOOP", this, &CAndy_State::NotTalking)

		.Finish_Setting();

	return S_OK;
}

void CAndy_State::Start_Cowering(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(COWERING_LOOP);
}

void CAndy_State::Start_Surprised(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(SURPRISED);
}

void CAndy_State::Start_Talk(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TALK_BASE_LOOP);
}

void CAndy_State::Tick_Cowering(_double dTimeDelta)
{
}

void CAndy_State::Tick_Surprised(_double dTimeDelta)
{
}

void CAndy_State::Tick_Talk(_double dTimeDelta)
{
}

void CAndy_State::End_Cowering(_double dTimeDelta)
{
}

void CAndy_State::End_Surprised(_double dTimeDelta)
{
}

void CAndy_State::End_Talk(_double dTimeDelta)
{
}

_bool CAndy_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CAndy_State::Surprised()
{
	if (m_pAndy->m_bPlayerDetected == true && m_bSurprised == false)
	{
		m_bSurprised = true;
		return true;
	}

	return false;
}

_bool CAndy_State::Talking()
{
	return m_pAndy->m_bNowTalking;
}

_bool CAndy_State::NotTalking()
{
	return !m_pAndy->m_bNowTalking;
}

CAndy_State * CAndy_State::Create(CAndy * pAndy, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	CAndy_State*	pInstance = new CAndy_State;

	if (FAILED(pInstance->Initialize(pAndy, pStateMachineCom, pModelCom, pTransformCom)))
	{
		MSG_BOX("Failed to Create : CAndy_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAndy_State::Free()
{
}