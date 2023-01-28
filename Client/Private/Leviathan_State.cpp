#include "stdafx.h"
#include "..\Public\Leviathan_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Leviathan.h"
#include "Sigrid.h"

#define	POSITION_A	XMVectorSet(149.f, -2.f, 775.5f, 1.f)
#define	POSITION_B	XMVectorSet(205.f, -2.f, 713.f, 1.f)
#define	POSITION_C	XMVectorSet(210.f, -2.f, 855.5f, 1.f)
#define	POSITION_D	XMVectorSet(282.343f, -2.f, 888.565f, 1.f)
#define	POSITION_E	XMVectorSet(352.f, -2.f, 828.f, 1.f)
#define	POSITION_F	XMVectorSet(298.f, -2.f, 764.773f, 1.f)

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
	FAILED_CHECK_RETURN(SetUp_State_Attack_Phase_1(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Attack_Phase_2(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Die(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_FlyMove(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Turn(), E_FAIL);

	m_pModelCom->Set_LerpTime(0.005f);

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
	{
		m_pLeviathan->m_bPlayerDetected = true;
		m_pLeviathan->m_pPlayer->Set_BossBattle(true);
	}

	for (auto& pSphereCollider : m_pLeviathan->m_pSphereCol)
	{
		if (m_pLeviathan->m_pPlayer->Collision_Body(pSphereCollider))
			m_pLeviathan->m_pPlayer->Collision_Event(m_pLeviathan);

		if (m_pLeviathan->m_pPlayer->Is_Attack() == true && m_pLeviathan->m_tStatus.dCurHitCoolTime >= m_pLeviathan->m_tStatus.dInitHitCoolTime)
		{
			if (m_pLeviathan->m_pPlayer->Collision_Net(pSphereCollider))
				m_pLeviathan->Collision_Event(m_pLeviathan->m_pPlayer);
		}
	}

	/* Debug */
	if (m_pGameInstance->Key_Down(DIK_F6))
		m_ePhase = PHASE_2;

	if (m_ePhase == PHASE_2)
	{
		m_pLeviathan->m_tStatus.dInitAttackCoolTime = 3.0;
		m_pModelCom->Set_WholeAnimationSpeed(40.0);
	}
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
		.Init_Changer(L"TURN_RIGHT_180", this, &CLeviathan_State::To_Target_Angle_R180)
		.Init_Changer(L"TURN_LEFT_180", this, &CLeviathan_State::To_Target_Angle_L180)
		.Init_Changer(L"TURN_RIGHT_90", this, &CLeviathan_State::To_Target_Angle_R90)
		.Init_Changer(L"TURN_LEFT_90", this, &CLeviathan_State::To_Target_Angle_L90)
		.Init_Changer(L"ATT_BITE", this, &CLeviathan_State::Ready_Attack_Bite)
		.Init_Changer(L"ATT_MOVE", this, &CLeviathan_State::Ready_Attack_Move)
		.Init_Changer(L"ATT_TAIL_WHIP", this, &CLeviathan_State::Ready_Attack_TailWhip)
		.Init_Changer(L"ATT_WATER_BALL", this, &CLeviathan_State::Ready_Attack_Water_Ball)
		.Init_Changer(L"ATT_WATER_BEAM_ST", this, &CLeviathan_State::Ready_Attack_Water_Beam)
		.Init_Changer(L"ATT_WATER_TORNADO", this, &CLeviathan_State::Ready_Attack_Water_Tornado)
		.Init_Changer(L"ATT_WING", this, &CLeviathan_State::Ready_Attack_Wing)
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
		.Init_Changer(L"ATT_SPLASH", this, &CLeviathan_State::Ready_Attack_Splash)
		.Init_Changer(L"ATT_SPIN", this, &CLeviathan_State::Ready_Attack_Spin)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"WARP_2")
		.Init_Start(this, &CLeviathan_State::Start_Warp_2)
		.Init_Tick(this, &CLeviathan_State::Tick_Warp_2)
		.Init_End(this, &CLeviathan_State::End_Warp_2)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Attack_Phase_1()
{
	m_pStateMachineCom->Add_State(L"ATT_BITE")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Bite)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Bite)
		.Init_End(this, &CLeviathan_State::End_Attack_Bite)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_MOVE")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Move)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Move)
		.Init_End(this, &CLeviathan_State::End_Attack_Move)
		.Init_Changer(L"ATT_MOVE_1", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_MOVE_1")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Move_1)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Move_1)
		.Init_End(this, &CLeviathan_State::End_Attack_Move_1)
		.Init_Changer(L"ATT_MOVE_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_MOVE_2")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Move_2)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Move_2)
		.Init_End(this, &CLeviathan_State::End_Attack_Move_2)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_SPLASH")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Splash)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Splash)
		.Init_End(this, &CLeviathan_State::End_Attack_Splash)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_TAIL_WHIP")
		.Init_Start(this, &CLeviathan_State::Start_Attack_TailWhip)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_TailWhip)
		.Init_End(this, &CLeviathan_State::End_Attack_TailWhip)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WATER_BALL")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Ball)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Ball)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Ball)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Attack_Phase_2()
{
	m_pStateMachineCom->Add_State(L"ATT_SPIN")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Spin)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Spin)
		.Init_End(this, &CLeviathan_State::End_Attack_Spin)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WATER_BEAM_ST")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Beam_Start)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Beam_Start)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Beam_Start)
		.Init_Changer(L"ATT_WATER_BEAM_RIGHT_LP", this, &CLeviathan_State::Target_On_Right)
		.Init_Changer(L"ATT_WATER_BEAM_LEFT_LP", this, &CLeviathan_State::Target_On_Left)

		.Add_State(L"ATT_WATER_BEAM_RIGHT_LP")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Beam_Right_Loop)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Beam_Right_Loop)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Beam_Right_Loop)
		.Init_Changer(L"ATT_WATER_BEAM_RIGHT_ED", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WATER_BEAM_RIGHT_ED")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Beam_Right_End)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Beam_Right_End)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Beam_Right_End)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WATER_BEAM_LEFT_LP")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Beam_Left_Loop)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Beam_Left_Loop)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Beam_Left_Loop)
		.Init_Changer(L"ATT_WATER_BEAM_LEFT_ED", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WATER_BEAM_LEFT_ED")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Beam_Left_End)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Beam_Left_End)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Beam_Left_End)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WATER_TORNADO")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Water_Tornado)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Water_Tornado)
		.Init_End(this, &CLeviathan_State::End_Attack_Water_Tornado)
		.Init_Changer(L"FLY_MOVE_1", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"ATT_WING")
		.Init_Start(this, &CLeviathan_State::Start_Attack_Wing)
		.Init_Tick(this, &CLeviathan_State::Tick_Attack_Wing)
		.Init_End(this, &CLeviathan_State::End_Attack_Wing)
		.Init_Changer(L"FLY_MOVE_1", this, &CLeviathan_State::Animation_Finish)

		.Finish_Setting();

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
	m_pStateMachineCom->Add_State(L"FLY_MOVE_1")
		.Init_Start(this, &CLeviathan_State::Start_Fly_Move_1)
		.Init_Tick(this, &CLeviathan_State::Tick_Fly_Move_1)
		.Init_End(this, &CLeviathan_State::End_Fly_Move_1)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::PosY_Less_Neg50)
		.Init_Changer(L"FLY_MOVE_2", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"FLY_MOVE_2")
		.Init_Start(this, &CLeviathan_State::Start_Fly_Move_2)
		.Init_Tick(this, &CLeviathan_State::Tick_Fly_Move_2)
		.Init_End(this, &CLeviathan_State::End_Fly_Move_2)
		.Init_Changer(L"WARP_2", this, &CLeviathan_State::PosY_Less_Neg50)
		.Init_Changer(L"FLY_MOVE_1", this, &CLeviathan_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CLeviathan_State::SetUp_State_Turn()
{
	m_pStateMachineCom->Add_State(L"TURN_RIGHT_180")
		.Init_Start(this, &CLeviathan_State::Start_Turn_Right_180)
		.Init_Tick(this, &CLeviathan_State::Tick_Turn_Right_180)
		.Init_End(this, &CLeviathan_State::End_Turn_Right_180)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"TURN_RIGHT_90")
		.Init_Start(this, &CLeviathan_State::Start_Turn_Right_90)
		.Init_Tick(this, &CLeviathan_State::Tick_Turn_Right_90)
		.Init_End(this, &CLeviathan_State::End_Turn_Right_90)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"TURN_LEFT_180")
		.Init_Start(this, &CLeviathan_State::Start_Turn_Left_180)
		.Init_Tick(this, &CLeviathan_State::Tick_Turn_Left_180)
		.Init_End(this, &CLeviathan_State::End_Turn_Left_180)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Add_State(L"TURN_LEFT_90")
		.Init_Start(this, &CLeviathan_State::Start_Turn_Left_90)
		.Init_Tick(this, &CLeviathan_State::Tick_Turn_Left_90)
		.Init_End(this, &CLeviathan_State::End_Turn_Left_90)
		.Init_Changer(L"IDLE_LP", this, &CLeviathan_State::Animation_Finish)

		.Finish_Setting();

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

	if (m_pModelCom->Get_LastAnimationIndex() == ATT_WATER_BEAM_RIGHT_ED)
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(30.f));

	if (m_pModelCom->Get_LastAnimationIndex() == ATT_WATER_BEAM_LEFT_ED)
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-30.f));

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
	if (m_iCurrentPattern == (_uint)PHASE_2_PATTERN_END)
	{
		srand(unsigned(time(NULL)));

		_vector	vPos;
		_matrix	matPlayerWorld = m_pLeviathan->m_pPlayer->Get_WorldMatrix();
		POSITION	eNextPos = POSITION(rand() % (_int)POS_PLAYER);

		while (eNextPos == m_eCurrentPos)
			eNextPos = POSITION(rand() % (_int)POS_PLAYER);

		switch (eNextPos)
		{
		case POS_A:
			vPos = POSITION_A;
			break;

		case POS_B:
			vPos = POSITION_B;
			break;

		case POS_C:
			vPos = POSITION_C;
			break;

		case POS_D:
			vPos = POSITION_D;
			break;

		case POS_E:
			vPos = POSITION_E;
			break;

		case POS_F:
			vPos = POSITION_F;
			break;

		case POS_G:
		{
			_vector	vPlayerPos = matPlayerWorld.r[3];
			_vector	vPlayerLook = XMVector3Normalize(XMVectorSetY(matPlayerWorld.r[2], 0.f));

			vPos = XMVectorSetY(vPlayerPos + vPlayerLook * 50.f, -2.f);
		}
		break;

		case POS_H:
		{
			_vector	vPlayerPos = matPlayerWorld.r[3];
			_vector	vPlayerLook = XMVector3Normalize(XMVectorSetY(matPlayerWorld.r[2], 0.f));

			vPos = XMVectorSetY(vPlayerPos + vPlayerLook * 50.f, -2.f);
		}
		break;

		case POS_END:
		{
			_vector	vPlayerPos = matPlayerWorld.r[3];

			vPos = XMVectorSetY(vPlayerPos, -2.f);
		}
		break;
		}

		m_eCurrentPos = eNextPos;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);

		if (m_eCurrentPos != POS_END)
			m_pTransformCom->LookAt_NoUpDown(matPlayerWorld.r[3]);
	}

	m_pModelCom->Set_CurAnimationIndex(WARP_2);
}

void CLeviathan_State::Start_Attack_Bite(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_BITE);
}

void CLeviathan_State::Start_Attack_Move(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_MOVE_1);
}

void CLeviathan_State::Start_Attack_Move_1(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_MOVE_2);
}

void CLeviathan_State::Start_Attack_Move_2(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_MOVE_3);
}

void CLeviathan_State::Start_Attack_Splash(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_SPLASH);
}

void CLeviathan_State::Start_Attack_TailWhip(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_TAIL_WHIP);
}

void CLeviathan_State::Start_Attack_Water_Ball(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_BALL);
}

void CLeviathan_State::Start_Attack_Spin(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_SPIN);
}

void CLeviathan_State::Start_Attack_Water_Beam_Start(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_BEAM_ST);
}

void CLeviathan_State::Start_Attack_Water_Beam_Right_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_BEAM_RIGHT_LP);
}

void CLeviathan_State::Start_Attack_Water_Beam_Right_End(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_BEAM_RIGHT_ED);
}

void CLeviathan_State::Start_Attack_Water_Beam_Left_Loop(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_BEAM_LEFT_LP);
}

void CLeviathan_State::Start_Attack_Water_Beam_Left_End(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_BEAM_LEFT_ED);
}

void CLeviathan_State::Start_Attack_Water_Tornado(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WATER_TORNADO);
}

void CLeviathan_State::Start_Attack_Wing(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(ATT_WING);
}

void CLeviathan_State::Start_Fly_Move_1(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == ATT_WATER_TORNADO)
	{
		_vector	vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS), -2.f);
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);
	}

	m_pModelCom->Set_CurAnimationIndex(FLY_MOVE_1);
}

void CLeviathan_State::Start_Fly_Move_2(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(FLY_MOVE_2);
}

void CLeviathan_State::Start_Turn_Right_180(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TURN_RIGHT_180);
}

void CLeviathan_State::Start_Turn_Right_90(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TURN_RIGHT_90);
}

void CLeviathan_State::Start_Turn_Left_180(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TURN_LEFT_180);
}

void CLeviathan_State::Start_Turn_Left_90(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationIndex(TURN_LEFT_90);
}

void CLeviathan_State::Tick_Appear(_double dTimeDelta)
{
	if (m_pLeviathan->m_bPlayerDetected == true)
	{
		_matrix	matPlayerWorld = m_pLeviathan->m_pPlayer->Get_WorldMatrix();

		m_pTransformCom->LookAt_NoUpDown(matPlayerWorld.r[3]);
	}
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
	if (m_pModelCom->Get_AnimationProgress() > 0.99f)
	{
		srand(unsigned(time(NULL)));

		Ready_Attack();

		_vector	vPos;
		_matrix	matPlayerWorld = m_pLeviathan->m_pPlayer->Get_WorldMatrix();
		POSITION	eNextPos = POS_END;

		if (m_iCurrentPattern == (_uint)BITE || m_iCurrentPattern == (_uint)TAIL_WHIP || m_iCurrentPattern == (_uint)WATER_TORNADO || m_iCurrentPattern == (_uint)WING)
		{
			eNextPos = POSITION(rand() % 2 + (_uint)POS_G);

			if (eNextPos == m_eCurrentPos)
			{
				if (m_eCurrentPos == POS_G)
					eNextPos = POS_H;
				else if (m_eCurrentPos == POS_H)
					eNextPos = POS_G;
			}
		}
		else if (m_iCurrentPattern == (_uint)MOVE || m_iCurrentPattern == (_uint)WATER_BALL || m_iCurrentPattern == (_uint)WATER_BEAM)
		{
			eNextPos = POSITION(rand() % (_uint)POS_G);

			while (eNextPos == m_eCurrentPos)
				eNextPos = POSITION(rand() % (_int)POS_G);
		}
		else if (m_iCurrentPattern == (_uint)SPLASH)
		{
			eNextPos = POS_PLAYER;
		}
		else
		{
			eNextPos = POSITION(rand() % (_int)POS_END);

			while (eNextPos == m_eCurrentPos)
				eNextPos = POSITION(rand() % (_int)POS_END);
		}

		switch (eNextPos)
		{
		case POS_A:
			vPos = POSITION_A;
			break;

		case POS_B:
			vPos = POSITION_B;
			break;

		case POS_C:
			vPos = POSITION_C;
			break;

		case POS_D:
			vPos = POSITION_D;
			break;

		case POS_E:
			vPos = POSITION_E;
			break;

		case POS_F:
			vPos = POSITION_F;
			break;

		case POS_G:
			{
				_vector	vPlayerPos = matPlayerWorld.r[3];
				_vector	vPlayerLook = XMVector3Normalize(XMVectorSetY(matPlayerWorld.r[2], 0.f));

				vPos = XMVectorSetY(vPlayerPos + vPlayerLook * 40.f, -2.f);
			}
			break;

		case POS_H:
			{
				_vector	vPlayerPos = matPlayerWorld.r[3];
				_vector	vPlayerLook = XMVector3Normalize(XMVectorSetY(matPlayerWorld.r[2], 0.f));

				vPos = XMVectorSetY(vPlayerPos + vPlayerLook * 40.f, -2.f);
			}
			break;

		case POS_PLAYER:
			{
				_vector	vPlayerPos = matPlayerWorld.r[3];

				vPos = XMVectorSetY(vPlayerPos, -2.f);
			}
			break;
		}

		m_eCurrentPos = eNextPos;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);

		if (m_eCurrentPos != POS_PLAYER)
			m_pTransformCom->LookAt_NoUpDown(matPlayerWorld.r[3]);
	}
}

void CLeviathan_State::Tick_Warp_2(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Bite(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Move(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Move_1(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Move_2(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Splash(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_TailWhip(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Ball(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Spin(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Beam_Start(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Beam_Right_Loop(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Beam_Right_End(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Beam_Left_Loop(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Beam_Left_End(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Attack_Water_Tornado(_double dTimeDelta)
{
	m_pTransformCom->Go_Down(dTimeDelta * 0.4f);
}

void CLeviathan_State::Tick_Attack_Wing(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Fly_Move_1(_double dTimeDelta)
{
	m_pTransformCom->Go_Down(dTimeDelta * 0.4f);
}

void CLeviathan_State::Tick_Fly_Move_2(_double dTimeDelta)
{
	m_pTransformCom->Go_Down(dTimeDelta * 0.4f);
}

void CLeviathan_State::Tick_Turn_Right_180(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Turn_Right_90(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Turn_Left_180(_double dTimeDelta)
{
}

void CLeviathan_State::Tick_Turn_Left_90(_double dTimeDelta)
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

void CLeviathan_State::End_Attack_Bite(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Move(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Move_1(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Move_2(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Splash(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_TailWhip(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Water_Ball(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Spin(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Water_Beam_Start(_double dTimeDelta)
{
}

void CLeviathan_State::End_Attack_Water_Beam_Right_Loop(_double dTimeDelta)
{
}

void CLeviathan_State::End_Attack_Water_Beam_Right_End(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Water_Beam_Left_Loop(_double dTimeDelta)
{
}

void CLeviathan_State::End_Attack_Water_Beam_Left_End(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Water_Tornado(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Attack_Wing(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Fly_Move_1(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Fly_Move_2(_double dTimeDelta)
{
	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	m_pLeviathan->m_tStatus.dCurAttackCoolTime = 0.0;
}

void CLeviathan_State::End_Turn_Right_180(_double dTimeDelta)
{
	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
}

void CLeviathan_State::End_Turn_Right_90(_double dTimeDelta)
{
	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
}

void CLeviathan_State::End_Turn_Left_180(_double dTimeDelta)
{
	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
}

void CLeviathan_State::End_Turn_Left_90(_double dTimeDelta)
{
	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
}

_bool CLeviathan_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack()
{
	if (m_iCurrentPattern != (_uint)PHASE_2_PATTERN_END)
		return false;

	if (m_pLeviathan->m_bPlayerDetected == true && m_pLeviathan->m_bReadyAttack == true)
	{
		srand(unsigned(time(NULL)));

		if (m_ePhase == PHASE_1)
		{
			m_iCurrentPattern = rand() % (_uint)PHASE_1_PATTERN_END;
		}
		else if (m_ePhase == PHASE_2)
		{
			m_iCurrentPattern = rand() % (_uint)PHASE_2_PATTERN_END;
		}

		/*if (m_iCurrentPattern != (_uint)MOVE && m_iCurrentPattern != (_uint)SPLASH && m_iCurrentPattern != (_uint)SPIN && m_iCurrentPattern != (_uint)WING)
		{
			if (Ready_Attack_Close() == true)
			{
				if (m_ePhase == PHASE_1)
				{
					m_iCurrentPattern = rand() % (_uint)PHASE_1_PATTERN_END;

					while (m_iCurrentPattern == m_iLastPattern || m_iCurrentPattern == (_uint)MOVE || m_iCurrentPattern == (_uint)SPLASH || m_iCurrentPattern == (_uint)WATER_BALL)
						m_iCurrentPattern = rand() % (_uint)PHASE_1_PATTERN_END;
				}
				else if (m_ePhase == PHASE_2)
				{
					m_iCurrentPattern = rand() % (_uint)PHASE_2_PATTERN_END;

					while (m_iCurrentPattern == m_iLastPattern || m_iCurrentPattern == (_uint)MOVE || m_iCurrentPattern == (_uint)SPLASH || m_iCurrentPattern == (_uint)SPIN || m_iCurrentPattern == (_uint)WING || m_iCurrentPattern == (_uint)WATER_BALL || m_iCurrentPattern == (_uint)WATER_BEAM)
						m_iCurrentPattern = rand() % (_uint)PHASE_2_PATTERN_END;
				}
			}
			else if (Ready_Attack_Far() == true)
			{
				if (m_ePhase == PHASE_1)
				{
					m_iCurrentPattern = rand() % (_uint)PHASE_1_PATTERN_END;

					while (m_iCurrentPattern == m_iLastPattern || m_iCurrentPattern == (_uint)MOVE || m_iCurrentPattern == (_uint)SPLASH || m_iCurrentPattern == (_uint)BITE || m_iCurrentPattern == (_uint)TAIL_WHIP)
						m_iCurrentPattern = rand() % (_uint)PHASE_1_PATTERN_END;
				}
				else if (m_ePhase == PHASE_2)
				{
					m_iCurrentPattern = rand() % (_uint)PHASE_2_PATTERN_END;

					while (m_iCurrentPattern == m_iLastPattern || m_iCurrentPattern == (_uint)MOVE || m_iCurrentPattern == (_uint)SPLASH || m_iCurrentPattern == (_uint)SPIN || m_iCurrentPattern == (_uint)WING || m_iCurrentPattern == (_uint)BITE || m_iCurrentPattern == (_uint)TAIL_WHIP || m_iCurrentPattern == (_uint)WATER_TORNADO)
						m_iCurrentPattern = rand() % (_uint)PHASE_2_PATTERN_END;
				}
			}
		}*/

		if (m_iCurrentPattern != (_uint)PHASE_2_PATTERN_END)
		{
			m_iLastPattern = m_iCurrentPattern;

			return true;
		}
	}

	m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Close()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];

	_float		fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPos));

	if (fDist < 45.f)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Far()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];

	_float		fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPos));

	if (fDist > 45.f)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Bite()
{
	if (m_iCurrentPattern == (_uint)BITE)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Move()
{
	if (m_iCurrentPattern == (_uint)MOVE)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Splash()
{
	if (m_iCurrentPattern == (_uint)SPLASH)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_TailWhip()
{
	if (m_iCurrentPattern == (_uint)TAIL_WHIP)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Water_Ball()
{
	if (m_iCurrentPattern == (_uint)WATER_BALL)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Spin()
{
	if (m_iCurrentPattern == (_uint)SPIN)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Water_Beam()
{
	if (m_iCurrentPattern == (_uint)WATER_BEAM)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Water_Tornado()
{
	if (m_iCurrentPattern == (_uint)WATER_TORNADO)
		return true;

	return false;
}

_bool CLeviathan_State::Ready_Attack_Wing()
{
	if (m_iCurrentPattern == (_uint)WING)
		return true;

	return false;
}

_bool CLeviathan_State::To_Target_Angle_R180()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector	vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float		fLRAngle = XMVectorGetX(XMVector3Dot(vDir, vRight));

	if (fLRAngle > 0.f)
	{
		_float		fAngle = XM_2PI - acosf(XMVectorGetX(XMVector3Dot(vDir, vLook)));

		if (fAngle > XMConvertToRadians(180.f) && fAngle < XMConvertToRadians(200.f))
			return true;
	}
	else
		return false;

	return false;
}

_bool CLeviathan_State::To_Target_Angle_L180()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector	vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float		fLRAngle = XMVectorGetX(XMVector3Dot(vDir, vRight));

	if (fLRAngle < 0.f)
	{
		_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(vDir, vLook)));

		if (fAngle > XMConvertToRadians(160.f) && fAngle < XMConvertToRadians(180.f))
			return true;
	}
	else
		return false;

	return false;
}

_bool CLeviathan_State::To_Target_Angle_R90()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector	vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float		fLRAngle = XMVectorGetX(XMVector3Dot(vDir, vRight));

	if (fLRAngle > 0.f)
	{
		_float		fAngle = XM_2PI - acosf(XMVectorGetX(XMVector3Dot(vDir, vLook)));

		if (fAngle > XMConvertToRadians(255.f) && fAngle < XMConvertToRadians(285.f))
			return true;
	}
	else
		return false;

	return false;
}

_bool CLeviathan_State::To_Target_Angle_L90()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector	vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	_float		fLRAngle = XMVectorGetX(XMVector3Dot(vDir, vRight));

	if (fLRAngle < 0.f)
	{
		_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(vDir, vLook)));

		if (fAngle > XMConvertToRadians(75.f) && fAngle < XMConvertToRadians(105.f))
			return true;
	}
	else
		return false;

	return false;
}

_bool CLeviathan_State::Target_On_Right()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	_float		fLRAngle = XMVectorGetX(XMVector3Dot(vDir, vRight));

	if (fLRAngle > 0.f)
		return true;

	return false;
}

_bool CLeviathan_State::Target_On_Left()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_vector	vTargetPos = XMLoadFloat4x4(&m_pLeviathan->m_pPlayer->Get_WorldMatrix()).r[3];
	_vector	vDir = XMVector3Normalize(vTargetPos - vPos);
	_vector	vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));

	_float		fLRAngle = XMVectorGetX(XMVector3Dot(vDir, vRight));

	if (fLRAngle < 0.f)
		return true;

	return false;
}

_bool CLeviathan_State::PosY_Less_Neg50()
{
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_float		fHeight = XMVectorGetY(vPos);

	if (fHeight < -50.f)
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