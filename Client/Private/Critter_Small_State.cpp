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
	m_pCritter->m_tStatus.dCurAttackCoolTime += dTimeDelta;

	if (m_pCritter->m_tStatus.dCurAttackCoolTime > m_pCritter->m_tStatus.dInitAttackCoolTime)
	{
		m_pCritter->m_bReadyAttack = true;
		m_pCritter->m_tStatus.dCurAttackCoolTime = m_pCritter->m_tStatus.dInitAttackCoolTime;
	}
	else
		m_pCritter->m_bReadyAttack = false;

	m_pCritter->m_tStatus.dCurHitCoolTime += dTimeDelta;

	if (m_pCritter->m_tStatus.dCurHitCoolTime > m_pCritter->m_tStatus.dInitHitCoolTime)
		m_pCritter->m_tStatus.dCurHitCoolTime = m_pCritter->m_tStatus.dInitHitCoolTime;
}

void CCritter_Small_State::Late_Tick(_double dTimeDelta)
{
	if (m_pCritter->m_pPlayer->Collision_Body(m_pCritter->m_pRangeCol))
		m_pCritter->m_bPlayerDetected = true;
	else
		m_pCritter->m_bPlayerDetected = false;

	if (m_pCritter->m_pPlayer->Collision_Body(m_pCritter->m_pSphereCol))
		m_pCritter->m_pPlayer->Collision_Event(m_pCritter);

	if (m_pCritter->m_pPlayer->Is_Attack() == true && m_pCritter->m_pPlayer->Collision_Net(m_pCritter->m_pSphereCol))
		m_pCritter->Collision_Event(m_pCritter->m_pPlayer);
}

HRESULT CCritter_Small_State::SetUp_State_Idle()
{
	m_pStateMachine->Set_Root(L"IDLE_STILL_LOOP")
		.Add_State(L"IDLE_STILL_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Idle)
		.Init_Tick(this, &CCritter_Small_State::Tick_Idle)
		.Init_End(this, &CCritter_Small_State::End_Idle)
		.Init_Changer(L"DEATH", this, &CCritter_Small_State::Dead)
		.Init_Changer(L"IDLE_STUN_LOOP", this, &CCritter_Small_State::Damaged_Special)
		.Init_Changer(L"STAGGER", this, &CCritter_Small_State::Damaged)
		.Init_Changer(L"TACKLE", this, &CCritter_Small_State::Ready_Attack)
		.Init_Changer(L"WALK_LOOP", this, &CCritter_Small_State::Check_IdleFinishCount)
		.Init_Changer(L"SPRINT_LOOP", this, &CCritter_Small_State::Player_DetectedAndFar)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Player_DetectedAndClose)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Stun()
{
	m_pStateMachine->Add_State(L"IDLE_STUN_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Stun)
		.Init_Tick(this, &CCritter_Small_State::Tick_Stun)
		.Init_End(this, &CCritter_Small_State::End_Stun)
		.Init_Changer(L"DEATH", this, &CCritter_Small_State::Dead)
		.Init_Changer(L"STUN_END", this, &CCritter_Small_State::Check_StunFinishCount)

		.Add_State(L"STUN_END")
		.Init_Start(this, &CCritter_Small_State::Start_Stun_End)
		.Init_Tick(this, &CCritter_Small_State::Tick_Stun_End)
		.Init_End(this, &CCritter_Small_State::End_Stun_End)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Walk()
{
	m_pStateMachine->Add_State(L"WALK_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Walk)
		.Init_Tick(this, &CCritter_Small_State::Tick_Walk)
		.Init_End(this, &CCritter_Small_State::End_Walk)
		.Init_Changer(L"DEATH", this, &CCritter_Small_State::Dead)
		.Init_Changer(L"IDLE_STUN_LOOP", this, &CCritter_Small_State::Damaged_Special)
		.Init_Changer(L"STAGGER", this, &CCritter_Small_State::Damaged)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Check_WalkTime)
		.Init_Changer(L"SPRINT_LOOP", this, &CCritter_Small_State::Player_DetectedAndFar)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Sprint()
{
	m_pStateMachine->Add_State(L"SPRINT_LOOP")
		.Init_Start(this, &CCritter_Small_State::Start_Sprint)
		.Init_Tick(this, &CCritter_Small_State::Tick_Sprint)
		.Init_End(this, &CCritter_Small_State::End_Sprint)
		.Init_Changer(L"DEATH", this, &CCritter_Small_State::Dead)
		.Init_Changer(L"IDLE_STUN_LOOP", this, &CCritter_Small_State::Damaged_Special)
		.Init_Changer(L"STAGGER", this, &CCritter_Small_State::Damaged)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Player_NotDetected)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Player_DetectedAndClose)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Tackle()
{
	m_pStateMachine->Add_State(L"TACKLE")
		.Init_Start(this, &CCritter_Small_State::Start_Tackle)
		.Init_Tick(this, &CCritter_Small_State::Tick_Tackle)
		.Init_End(this, &CCritter_Small_State::End_Tackle)
		.Init_Changer(L"DEATH", this, &CCritter_Small_State::Dead)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Stagger()
{
	m_pStateMachine->Add_State(L"STAGGER")
		.Init_Start(this, &CCritter_Small_State::Start_Stagger)
		.Init_Tick(this, &CCritter_Small_State::Tick_Stagger)
		.Init_End(this, &CCritter_Small_State::End_Stagger)
		.Init_Changer(L"IDLE_STUN_LOOP", this, &CCritter_Small_State::Damaged_Special)
		.Init_Changer(L"STAGGER", this, &CCritter_Small_State::Damaged)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::Animation_Finish)
		
		.Finish_Setting();

	return S_OK;
}

HRESULT CCritter_Small_State::SetUp_State_Death()
{
	m_pStateMachine->Add_State(L"DEATH")
		.Init_Start(this, &CCritter_Small_State::Start_Death)
		.Init_Tick(this, &CCritter_Small_State::Tick_Death)
		.Init_End(this, &CCritter_Small_State::End_Death)
		.Init_Changer(L"IDLE_STILL_LOOP", this, &CCritter_Small_State::KeyDown_F5)
		.Init_Changer(L"DEATH", this, &CCritter_Small_State::Animation_Finish)

		.Finish_Setting();

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
	m_pCritter->m_bDead = false;
}

void CCritter_Small_State::Start_Stun(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(IDLE_STUN_LOOP);
	m_iStunFinishCount = 0;
}

void CCritter_Small_State::Start_Stun_End(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(STUN_END);
}

void CCritter_Small_State::Start_Walk(_double dTimeDelta)
{
	m_dWalkTime = 0.0;
	m_pModelCom->Set_CurAnimationIndex(WALK_LOOP);
}

void CCritter_Small_State::Start_Sprint(_double dTimeDelta)
{
	m_pTransformCom->Set_Speed(5.0);
	m_pModelCom->Set_CurAnimationIndex(SPRINT_LOOP);
}

void CCritter_Small_State::Start_Tackle(_double dTimeDelta)
{
	m_pTransformCom->Set_Speed(1.0);
	m_pModelCom->Set_CurAnimationIndex(TACKLE);
}

void CCritter_Small_State::Start_Stagger(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(STAGGER);
}

void CCritter_Small_State::Start_Death(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(DEATH);
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

		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (_double)XMConvertToRadians(_float(rand() % 180 + 180) * (_float)dTimeDelta * 0.4f * fCW));
	}
}

void CCritter_Small_State::Tick_Stun(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationFinish())
		m_iStunFinishCount++;
}

void CCritter_Small_State::Tick_Stun_End(_double dTimeDelta)
{
}

void CCritter_Small_State::Tick_Walk(_double dTimeDelta)
{
	m_dWalkTime += dTimeDelta;

	m_pTransformCom->Go_Straight(dTimeDelta, m_pCritter->m_pNavigationCom);
}

void CCritter_Small_State::Tick_Sprint(_double dTimeDelta)
{
	_vector	vPlayerPos = XMLoadFloat4x4(&m_pCritter->m_pPlayer->Get_WorldMatrix()).r[3];

	m_pTransformCom->LookAt_NoUpDown(vPlayerPos);
	m_pTransformCom->Chase(vPlayerPos, dTimeDelta, 2.f, m_pCritter->m_pNavigationCom);
}

void CCritter_Small_State::Tick_Tackle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.4f)
		m_pTransformCom->Go_Straight(dTimeDelta);
}

void CCritter_Small_State::Tick_Stagger(_double dTimeDelta)
{
}

void CCritter_Small_State::Tick_Death(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Idle(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Stun(_double dTImeDelta)
{
}

void CCritter_Small_State::End_Stun_End(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Walk(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Sprint(_double dTimeDelta)
{
	m_pTransformCom->Set_Speed(3.0);
}

void CCritter_Small_State::End_Tackle(_double dTimeDelta)
{
	m_pCritter->m_tStatus.dCurAttackCoolTime = 0.0;
	m_pTransformCom->Set_Speed(3.0);
}

void CCritter_Small_State::End_Stagger(_double dTimeDelta)
{
}

void CCritter_Small_State::End_Death(_double dTimeDelta)
{
	m_pCritter->m_bDead = true;
}

_bool CCritter_Small_State::KeyDown_F5()
{
	if (m_pGameInstance->Key_Down(DIK_F5))
	{
		m_pCritter->m_tStatus.iHP = m_pCritter->m_tStatus.iMaxHP;
		return true;
	}

	return false;
}

_bool CCritter_Small_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CCritter_Small_State::Ready_Attack()
{
	_vector	vPlayerPos = XMLoadFloat4x4(&m_pCritter->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vCritterPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	_float		fDist = XMVectorGetX(XMVector3Length(vPlayerPos - vCritterPos));

	if (m_pCritter->m_bHit == false && m_pCritter->m_bReadyAttack == true && m_pCritter->m_bPlayerDetected && fDist <= 2.f)
		return true;

	return false;
}

_bool CCritter_Small_State::Damaged()
{
	return m_pCritter->m_bHit == true && m_pCritter->m_pPlayer->Is_SpecialAttack() == false;
}

_bool CCritter_Small_State::Damaged_Special()
{
	return m_pCritter->m_bHit == true && m_pCritter->m_pPlayer->Is_SpecialAttack() == true;
}

_bool CCritter_Small_State::Dead()
{
	return m_pCritter->m_tStatus.iHP <= 0;
}

_bool CCritter_Small_State::Player_Detected()
{
	return m_pCritter->m_bPlayerDetected;
}

_bool CCritter_Small_State::Player_DetectedAndFar()
{
	_vector	vPlayerPos = XMLoadFloat4x4(&m_pCritter->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vCritterPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	_vector	vDir = XMVectorSetY(vPlayerPos - vCritterPos, 0.f);

	_float		fDist = XMVectorGetX(XMVector3Length(vDir));

	if (m_pCritter->m_bPlayerDetected && fDist > 2.f)
		return true;

	return false;
}

_bool CCritter_Small_State::Player_DetectedAndClose()
{
	_vector	vPlayerPos = XMLoadFloat4x4(&m_pCritter->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vCritterPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	_vector	vDir = XMVectorSetY(vPlayerPos - vCritterPos, 0.f);

	_float		fDist = XMVectorGetX(XMVector3Length(vDir));

	if (m_pCritter->m_bPlayerDetected && fDist <= 2.f)
		return true;

	return false;
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

_bool CCritter_Small_State::Check_StunFinishCount()
{
	if (m_iStunFinishCount > 1)
		return true;

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