#include "stdafx.h"
#include "..\Public\Baine_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Baine.h"
#include "Sigrid.h"

CBaine_State::CBaine_State()
{
}

HRESULT CBaine_State::Initialize(CBaine * pBaine, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pBaine = pBaine;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModelCom;
	m_pTransformCom = pTransformCom;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Surprised(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Talk(), E_FAIL);

	return S_OK;
}

void CBaine_State::Tick(_double dTimeDelta)
{
}

void CBaine_State::Late_Tick(_double dTImeDelta)
{
	if (m_pBaine->m_pPlayer->Collision_Body(m_pBaine->m_pRangeCol))
	{
		m_pBaine->m_bPlayerDetected = true;
	}
	else
	{
		m_pBaine->m_bPlayerDetected = false;
		m_bSurprised = false;
	}
}

HRESULT CBaine_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE_BASE_LOOP")
		.Add_State(L"IDLE_BASE_LOOP")
		.Init_Start(this, &CBaine_State::Start_Idle_Base_Loop)
		.Init_Tick(this, &CBaine_State::Tick_Idle_Base_Loop)
		.Init_End(this, &CBaine_State::End_Idle_Base_Loop)
		.Init_Changer(L"SURPRISED", this, &CBaine_State::Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CBaine_State::Talking)

		.Add_State(L"IDLE_HAPPY_LOOP")
		.Init_Start(this, &CBaine_State::Start_Idle_Happy_Loop)
		.Init_Tick(this, &CBaine_State::Tick_Idle_Happy_Loop)
		.Init_End(this, &CBaine_State::End_Idle_Happy_Loop)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CBaine_State::Talking)
		.Init_Changer(L"IDLE_BASE_LOOP", this, &CBaine_State::Animation_Finish)

		.Add_State(L"IDLE_STILL_LOOP")
		.Init_Start(this, &CBaine_State::Start_Idle_Still_Loop)
		.Init_Tick(this, &CBaine_State::Tick_Idle_Still_Loop)
		.Init_End(this, &CBaine_State::End_Idle_Still_Loop)

		.Finish_Setting();

	return S_OK;
}

HRESULT CBaine_State::SetUp_State_Surprised()
{
	m_pStateMachine->Add_State(L"SURPRISED")
		.Init_Start(this, &CBaine_State::Start_Surprised)
		.Init_Tick(this, &CBaine_State::Tick_Surprised)
		.Init_End(this, &CBaine_State::End_Surprised)
		.Init_Changer(L"TALK_BASE_LOOP", this, &CBaine_State::Talking)
		.Init_Changer(L"IDLE_BASE_LOOP", this, &CBaine_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CBaine_State::SetUp_State_Talk()
{
	m_pStateMachine->Add_State(L"TALK_BASE_LOOP")
		.Init_Start(this, &CBaine_State::Start_Talk)
		.Init_Tick(this, &CBaine_State::Tick_Talk)
		.Init_End(this, &CBaine_State::End_Talk)
		.Init_Changer(L"IDLE_HAPPY_LOOP", this, &CBaine_State::ItemPurchased)

		.Finish_Setting();

	return S_OK;
}

void CBaine_State::Start_Idle_Base_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_BASE_LOOP);
}

void CBaine_State::Start_Idle_Happy_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_HAPPY_LOOP);
	m_pBaine->m_pPlayer->Set_ItemPurchased(false);
}

void CBaine_State::Start_Idle_Still_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_STILL_LOOP);
}

void CBaine_State::Start_Surprised(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(SURPRISED);
}

void CBaine_State::Start_Talk(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TALK_BASE_LOOP);
}

void CBaine_State::Tick_Idle_Base_Loop(_double dTimeDelta)
{
}

void CBaine_State::Tick_Idle_Happy_Loop(_double dTimeDelta)
{
}

void CBaine_State::Tick_Idle_Still_Loop(_double dTimeDelta)
{
}

void CBaine_State::Tick_Surprised(_double dTimeDelta)
{
}

void CBaine_State::Tick_Talk(_double dTimeDelta)
{
}

void CBaine_State::End_Idle_Base_Loop(_double dTimeDelta)
{
}

void CBaine_State::End_Idle_Happy_Loop(_double dTimeDelta)
{
}

void CBaine_State::End_Idle_Still_Loop(_double dTimeDelta)
{
}

void CBaine_State::End_Surprised(_double dTimeDelta)
{
}

void CBaine_State::End_Talk(_double dTimeDelta)
{
}

_bool CBaine_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CBaine_State::Surprised()
{
	if (m_pBaine->m_bPlayerDetected == true && m_bSurprised == false)
	{
		m_bSurprised = true;
		return true;
	}

	return false;
}

_bool CBaine_State::Talking()
{
	return m_pBaine->m_bNowTalking;
}

_bool CBaine_State::ItemPurchased()
{
	return (m_pBaine->m_pPlayer->Get_ItemPurchased() == true) && (g_bShopOpen == false);
}

CBaine_State * CBaine_State::Create(CBaine * pBaine, CStateMachine * pStateMachineCom, CModel * pModelCom, CTransform * pTransformCom)
{
	CBaine_State*	pInstance = new CBaine_State;

	if (FAILED(pInstance->Initialize(pBaine, pStateMachineCom, pModelCom, pTransformCom)))
	{
		MSG_BOX("Failed to Create : CBaine_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBaine_State::Free()
{
}