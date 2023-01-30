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

	return S_OK;
}

void CBaine_State::Tick(_double dTimeDelta)
{
}

void CBaine_State::Late_Tick(_double dTImeDelta)
{
	if (m_pBaine->m_pPlayer->Collision_Body(m_pBaine->m_pRangeCol))
		m_pBaine->m_bPlayerDetected = true;
	else
		m_pBaine->m_bPlayerDetected = false;
}

HRESULT CBaine_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE_BASE_LOOP")
		.Add_State(L"IDLE_BASE_LOOP")
		.Init_Start(this, &CBaine_State::Start_Idle_Base_Loop)
		.Init_Tick(this, &CBaine_State::Tick_Idle_Base_Loop)
		.Init_End(this, &CBaine_State::End_Idle_Base_Loop)

		.Add_State(L"IDLE_HAPPY_LOOP")
		.Init_Start(this, &CBaine_State::Start_Idle_Happy_Loop)
		.Init_Tick(this, &CBaine_State::Tick_Idle_Happy_Loop)
		.Init_End(this, &CBaine_State::End_Idle_Happy_Loop)

		.Add_State(L"IDLE_STILL_LOOP")
		.Init_Start(this, &CBaine_State::Start_Idle_Still_Loop)
		.Init_Tick(this, &CBaine_State::Tick_Idle_Still_Loop)
		.Init_End(this, &CBaine_State::End_Idle_Still_Loop)

		.Finish_Setting();

	return S_OK;
}

void CBaine_State::Start_Idle_Base_Loop(_double dTimeDelta)
{
}

void CBaine_State::Start_Idle_Happy_Loop(_double dTimeDelta)
{
}

void CBaine_State::Start_Idle_Still_Loop(_double dTimeDelta)
{
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

void CBaine_State::End_Idle_Base_Loop(_double dTimeDelta)
{
}

void CBaine_State::End_Idle_Happy_Loop(_double dTimeDelta)
{
}

void CBaine_State::End_Idle_Still_Loop(_double dTimeDelta)
{
}

_bool CBaine_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
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