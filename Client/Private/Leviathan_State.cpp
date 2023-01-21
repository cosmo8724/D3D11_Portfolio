#include "stdafx.h"
#include "..\Public\Leviathan_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Leviathan.h"
#include "Sigrid.h"

CLeviathan_State::CLeviathan_State()
{
}

HRESULT CLeviathan_State::Initialize(DEVICE pDevice, DEVICE_CONTEXT pContext, CLeviathan * pLeviathan, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = CGameInstance::GetInstance();

	m_pLeviathan = pLeviathan;
	m_pStateMachineCom = pStateMachineCom;
	m_pModelCom = pModel;
	m_pTransformCom = pTransform;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Warp(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack_Energy_Charge(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack_Move(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack_Water_Beam(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Die(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_FlyMove(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Turn(), E_FAIL);

	return S_OK;
}

void CLeviathan_State::Tick(_double dTimeDelta)
{
	m_pLeviathan->m_tStatus.dCurAttackCoolTime += dTimeDelta;

	if (m_pLeviathan->m_tStatus.dCurAttackCoolTime > m_pLeviathan->m_tStatus.dInitAttackCoolTime)
	{
		m_pLeviathan->m_bReadyAttack = true;
		m_pLeviathan->m_tStatus.dCurAttackCoolTime = m_pLeviathan->m_tStatus.dInitAttackCoolTime;
	}
	else
		m_pLeviathan->m_bReadyAttack = false;

	m_pLeviathan->m_tStatus.dCurHitCoolTime += dTimeDelta;

	if (m_pLeviathan->m_tStatus.dCurHitCoolTime > m_pLeviathan->m_tStatus.dInitHitCoolTime)
		m_pLeviathan->m_tStatus.dCurHitCoolTime = m_pLeviathan->m_tStatus.dInitHitCoolTime;
}

void CLeviathan_State::Late_Tick(_double dTimeDelta)
{
	if (m_pLeviathan->m_pPlayer->Collision_Body(m_pLeviathan->m_pRangeCol))
		m_pLeviathan->m_bPlayerDetected = true;

	
}

HRESULT CLeviathan_State::SetUp_State_Idle()
{
	m_pStateMachineCom->Set_Root(L"APPEAR")
		.Add_State(L"APPEAR")
		.Init_Start(this, &CLeviathan_State::Start_Appear)
		.Init_Tick(this, &CLeviathan_State::Tick_Appear)
		.Init_End(this, &CLeviathan_State::End_Appear)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"IDLE_LP")
		.Init_Start(this, &CLeviathan_State::Start_Idle_Loop)
		.Init_Tick(this, &CLeviathan_State::Tick_Idle_Loop)
		.Init_End(this, &CLeviathan_State::End_Idle_Loop)
		.Init_Changer(L"WARP_1", this, &CLeviathan_State::Check_IdleFinishCount)

		.Finish_Setting();

	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Warp()
{
	m_pStateMachineCom->Add_State(L"WARP_1")
		.Init_Start(this, &CLeviathan_State::Start_Warp_1)
		.Init_Tick(this, &CLeviathan_State::Tick_Warp_1)
		.Init_End(this, &CLeviathan_State::End_Warp_1)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"WARP_2")
		.Init_Start(this, &CLeviathan_State::Start_Warp_2)
		.Init_Tick(this, &CLeviathan_State::Tick_Warp_2)
		.Init_End(this, &CLeviathan_State::End_Warp_2)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Attack()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Attack_Energy_Charge()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Attack_Move()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Attack_Water_Beam()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Damaged()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Die()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_FlyMove()
{
	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Turn()
{
	return S_OK;
}

void CLeviathan_State::Start_Appear(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(APPEAR);
}

void CLeviathan_State::Start_Idle_Start(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_ST);
}

void CLeviathan_State::Start_Idle_Loop(_double dTimeDelta)
{
	m_iIdleFinishCount = 0;
	m_pModelCom->Set_CurAnimationIndex(IDLE_LP);
}

void CLeviathan_State::Start_Idle_End(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_ED);
}

void CLeviathan_State::Start_Warp_1(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(WARP_1);
}

void CLeviathan_State::Start_Warp_2(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(WARP_2);
}

void CLeviathan_State::Tick_Appear(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Idle_Start(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Idle_Loop(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationFinish())
		m_iIdleFinishCount++;
}

void CLeviathan_State::Tick_Idle_End(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Warp_1(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Warp_2(_double dTimeDelta)
{
}

void CLeviathan_State::End_Appear(_double dTimeDelta)
{
}

void CLeviathan_State::End_Idle_Start(_double dTimeDelta)
{
}

void CLeviathan_State::End_Idle_Loop(_double dTimeDelta)
{
}

void CLeviathan_State::End_Idle_End(_double dTimeDelta)
{
}

void CLeviathan_State::End_Warp_1(_double dTimeDelta)
{
}

void CLeviathan_State::End_Warp_2(_double dTimeDelta)
{
}

_bool CLeviathan_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CLeviathan_State::Check_IdleFinishCount()
{
	if (m_iIdleFinishCount > 1)
		return  true;

	return false;
}

CLeviathan_State * CLeviathan_State::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CLeviathan * pLeviathan, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	CLeviathan_State*	pInstance = new CLeviathan_State;

	if (FAILED(pInstance->Initialize(pDevice, pContext, pLeviathan, pStateMachineCom, pModel, pTransform)))
	{
		MSG_BOX("Failed to Create : CLeviathan_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeviathan_State::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}