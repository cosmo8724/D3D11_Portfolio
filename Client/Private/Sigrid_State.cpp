#include "stdafx.h"
#include "..\Public\Sigrid_State.h"
#include "GameInstance.h"
#include "Sigrid.h"
#include "Static_Camera.h"
#include "GameUtility.h"

/*
FSM 추후 수정 해야할 것

Water -> Surf 가는 법 추가
Jump_Charged 시 땅 아래로 파고드는 것
Ground_Run 발이 너무 부자연 스러움
Diving 추가
Net 폈다 접었다 추가
Grapple 액션 추가
Damaged 추가
*/

CSigrid_State::CSigrid_State()
{
}

HRESULT CSigrid_State::Initialize(CSigrid * pPlayer, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform, CStatic_Camera * pCamera)
{
	m_pGameInstance = CGameInstance::GetInstance();

	m_pPlayer = pPlayer;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModel;
	m_pTransformCom = pTransform;
	m_pCamera = pCamera;

	FAILED_CHECK_RETURN(SetUp_State_Ground_Idle(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Ground_Run(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Ground_Boost(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Water(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Surf(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Surf_Boost(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Dash(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Snap_Turn(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Jump(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Jump_Double(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Jump_Charge(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Air(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Air_Gliding(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Landing(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Combat_Combo(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Combat_Charge_Attack(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Combat_Ground_Slam(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Damaged(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Grapple_Ground(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Grapple_Air(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Grapple_Aim(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Grapple_Hang(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_State_Grapple_Launch(), E_FAIL);

	return S_OK;
}

void CSigrid_State::Tick(_double & dTimeDelta)
{
	m_eDir = DetectDirectionInput();

	m_pPlayer->m_dPauseTime += dTimeDelta;

	m_pPlayer->m_tStatus.dCurHitCoolTime += dTimeDelta;

	if (m_pPlayer->m_tStatus.dCurHitCoolTime > m_pPlayer->m_tStatus.dInitHitCoolTime)
		m_pPlayer->m_tStatus.dCurHitCoolTime = m_pPlayer->m_tStatus.dInitHitCoolTime;

	if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) == false)
		m_pPlayer->m_bBoost = false;

	if (m_pPlayer->m_bReadyLaunch == true)
	{
		m_pTransformCom->Set_Speed(40.0);
		m_pPlayer->m_fInertia = 1.5f;
	}
	else
	{
		if (m_pPlayer->m_bBoost == false)
		{
			if (m_pPlayer->m_bJump == false)
				m_pTransformCom->Set_Speed(15.0);

			m_pPlayer->m_fInertia = 5.f;

			if (m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING ||
				m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING_LEFT ||
				m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING_RIGHT)
				m_pPlayer->m_fInertia *= 0.7f;
		}
		else
		{
			if (m_pPlayer->m_bJump == false)
				m_pTransformCom->Set_Speed(25.0);
			
			m_pPlayer->m_fInertia = 1.5f;
		}

		if (m_pPlayer->m_bOnOcean)
		{
			m_pPlayer->m_dSurfTime += dTimeDelta;

			if (m_pModelCom->Get_CurAnimationIndex() == WATER_IDLE ||
				m_pModelCom->Get_CurAnimationIndex() == GROUND_RUN ||
				m_pModelCom->Get_CurAnimationIndex() == GROUND_RUN_LEFT ||
				m_pModelCom->Get_CurAnimationIndex() == GROUND_RUN_RIGHT)
			{
				m_pPlayer->m_dSurfTime = 0.0;

				if (m_pPlayer->m_bJump == false)
					m_pTransformCom->Set_Speed(12.5);
			}

			if (m_pPlayer->m_dSurfTime > 3.0 && m_pPlayer->m_bBoost == false)
			{
				if (m_pPlayer->m_bJump == false)
					m_pTransformCom->Set_Speed(20.0);

				m_pPlayer->m_fInertia = 2.f;
			}

			if (m_pPlayer->m_bJump == false)
			{
				_double&	dSpeed = m_pTransformCom->Get_Speed();
				dSpeed *= 1.5;
			}

			m_pPlayer->m_fInertia *= 0.7f;
		}
	}

	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (vPlayerPos.y <= m_pPlayer->m_fGroundHeight && m_pStateMachine->Get_CurrentState() == L"AIR" && m_pPlayer->m_bJump == false)
 		m_pPlayer->m_bJump = true;
}

void CSigrid_State::Late_Tick(_double & dTimeDelta)
{
	m_eLastDir = m_eDir;
}

void CSigrid_State::ImGui_RenderProperty()
{
	ImGui::InputFloat("Left", &m_fTestLeft, 0.0001f, 0.001f, "%.4f");
	ImGui::InputFloat("Right", &m_fTestRight, 0.0001f, 0.001f, "%.4f");
}

HRESULT CSigrid_State::SetUp_State_Ground_Idle()
{
	m_pStateMachine->Set_Root(L"GROUND_IDLE")
		.Add_State(L"GROUND_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Ground_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Idle)
		.Init_End(this, &CSigrid_State::End_Ground_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SNAP_TURN_GROUND_IDLE", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_IDLE", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Ground_Run()
{
	m_pStateMachine->
		Add_State(L"GROUND_RUN")
		.Init_Start(this, &CSigrid_State::Start_Ground_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Run)
		.Init_End(this, &CSigrid_State::End_Ground_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_RUN_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"GROUND_RUN_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_RUN", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"SNAP_TURN_GROUND_RUN", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_RUN", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"GROUND_RUN_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Ground_Run_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Run_Left)
		.Init_End(this, &CSigrid_State::End_Ground_Run_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"GROUND_RUN_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"GROUND_BOOST_LEFT", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_RUN_LEFT", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"SNAP_TURN_GROUND_RUN", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_RUN", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"GROUND_RUN_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Ground_Run_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Run_Right)
		.Init_End(this, &CSigrid_State::End_Ground_Run_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"GROUND_RUN_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"GROUND_BOOST_RIGHT", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_RUN_RIGHT", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"SNAP_TURN_GROUND_RUN", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_RUN", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Ground_Boost()
{
	m_pStateMachine->
		Add_State(L"GROUND_BOOST")
		.Init_Start(this, &CSigrid_State::Start_Ground_Boost)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Boost)
		.Init_End(this, &CSigrid_State::End_Ground_Boost)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyUp_Shift)
		.Init_Changer(L"GROUND_BOOST_STOP", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"GROUND_BOOST_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"SURF_FAST_INTRO", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"SNAP_TURN_GROUND_BOOST", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)

		.Add_State(L"GROUND_BOOST_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Ground_Boost_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Boost_Left)
		.Init_End(this, &CSigrid_State::End_Ground_Boost_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyUp_Shift)
		.Init_Changer(L"GROUND_BOOST", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"GROUND_BOOST_STOP", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"SURF_FAST_INTRO", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"SNAP_TURN_GROUND_BOOST", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)

		.Add_State(L"GROUND_BOOST_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Ground_Boost_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Boost_Right)
		.Init_End(this, &CSigrid_State::End_Ground_Boost_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyUp_Shift)
		.Init_Changer(L"GROUND_BOOST", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"GROUND_BOOST_STOP", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"SURF_FAST_INTRO", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"SNAP_TURN_GROUND_BOOST", this, &CSigrid_State::Turn_Back)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)

		.Add_State(L"GROUND_BOOST_STOP")
		.Init_Start(this, &CSigrid_State::Start_Ground_Boost_Stop)
		.Init_Tick(this, &CSigrid_State::Tick_Ground_Boost_Stop)
		.Init_End(this, &CSigrid_State::End_Ground_Boost_Stop)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::IsOnOcean)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Water()
{
	m_pStateMachine->
		Add_State(L"WATER_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Water_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Idle)
		.Init_End(this, &CSigrid_State::End_Water_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"WATER_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_WATER_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"WATER_RUN")
		.Init_Start(this, &CSigrid_State::Start_Water_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Run)
		.Init_End(this, &CSigrid_State::End_Water_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"WATER_RUN_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"WATER_RUN_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"WATER_RUN_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Water_Run_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Run_Left)
		.Init_End(this, &CSigrid_State::End_Water_Run_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"WATER_RUN", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"WATER_RUN_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN_LEFT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"JUMP_CHARGING_LEFT", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"WATER_RUN_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Water_Run_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Run_Right)
		.Init_End(this, &CSigrid_State::End_Water_Run_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"WATER_RUN", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"WATER_RUN_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN_RIGHT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"JUMP_CHARGING_RIGHT", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"WATER_BRAKING")
		.Init_Start(this, &CSigrid_State::Start_Water_Braking)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Braking)
		.Init_End(this, &CSigrid_State::End_Water_Braking)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"WATER_DIVING_EXIT")
		.Init_Start(this, &CSigrid_State::Start_Water_Diving_Exit)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Diving_Exit)
		.Init_End(this, &CSigrid_State::End_Water_Diving_Exit)

		.Add_State(L"WATER_DIVING")
		.Init_Start(this, &CSigrid_State::Start_Water_Diving)
		.Init_Tick(this, &CSigrid_State::Tick_Water_Diving)
		.Init_End(this, &CSigrid_State::End_Water_Diving)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Surf()
{
	m_pStateMachine->
		Add_State(L"SURF_INTRO")
		.Init_Start(this, &CSigrid_State::Start_Surf_Intro)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Intro)
		.Init_End(this, &CSigrid_State::End_Surf_Intro)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF")
		.Init_Start(this, &CSigrid_State::Start_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Surf)
		.Init_End(this, &CSigrid_State::End_Surf)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"SURF_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Check_SurfTime)
		.Init_Changer(L"SURF_BOOST", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Surf_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Left)
		.Init_End(this, &CSigrid_State::End_Surf_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"SURF_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"SURF_FAST_LEFT", this, &CSigrid_State::Check_SurfTime)
		.Init_Changer(L"SURF_BOOST_LEFT", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN_LEFT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING_LEFT", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Surf_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Right)
		.Init_End(this, &CSigrid_State::End_Surf_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"SURF_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"SURF_FAST_RIGHT", this, &CSigrid_State::Check_SurfTime)
		.Init_Changer(L"SURF_BOOST_RIGHT", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN_RIGHT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING_RIGHT", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF_FAST_INTRO")
		.Init_Start(this, &CSigrid_State::Start_Surf_Fast_Intro)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Fast_Intro)
		.Init_End(this, &CSigrid_State::End_Surf_Fast_Intro)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF_FAST")
		.Init_Start(this, &CSigrid_State::Start_Surf_Fast)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Fast)
		.Init_End(this, &CSigrid_State::End_Surf_Fast)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"SURF_FAST_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"SURF_BOOST", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF_FAST_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Surf_Fast_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Fast_Left)
		.Init_End(this, &CSigrid_State::End_Surf_Fast_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"SURF_FAST_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"SURF_BOOST_LEFT", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN_LEFT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING_LEFT", this, &CSigrid_State::KeyInput_Space)

		.Add_State(L"SURF_FAST_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Surf_Fast_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Fast_Right)
		.Init_End(this, &CSigrid_State::End_Surf_Fast_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"SURF_FAST_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"SURF_BOOST_RIGHT", this, &CSigrid_State::KeyInput_Shift)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN_RIGHT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING_RIGHT", this, &CSigrid_State::KeyInput_Space)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Surf_Boost()
{
	m_pStateMachine->
		Add_State(L"SURF_BOOST")
		.Init_Start(this, &CSigrid_State::Start_Surf_Boost)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Boost)
		.Init_End(this, &CSigrid_State::End_Surf_Boost)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::KeyUp_Shift)
		.Init_Changer(L"SURF_BOOST_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"SURF_BOOST_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"SURF_BOOST_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Surf_Boost_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Boost_Left)
		.Init_End(this, &CSigrid_State::End_Surf_Boost_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::KeyUp_Shift)
		.Init_Changer(L"SURF_BOOST", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"SURF_BOOST_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST_LEFT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"SURF_BOOST_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Surf_Boost_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Surf_Boost_Right)
		.Init_End(this, &CSigrid_State::End_Surf_Boost_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::KeyUp_Shift)
		.Init_Changer(L"SURF_BOOST", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"SURF_BOOST_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"WATER_BRAKING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST_RIGHT", this, &CSigrid_State::IsOnGround)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Dash()
{
	m_pStateMachine->
		Add_State(L"DASH_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Dash_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Dash_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Dash_Into_Idle)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyInput_Direction_Progress)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"DASH_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Dash_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Dash_Into_Run)
		.Init_End(this, &CSigrid_State::End_Dash_Into_Run)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::IsOnAir)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"DASH_INTO_AIR")
		.Init_Start(this, &CSigrid_State::Start_Dash_Into_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Dash_Into_Air)
		.Init_End(this, &CSigrid_State::End_Dash_Into_Air)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space_Progress)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDown_Space_While_Dash_Into_Air)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"DASH_INTO_WATER_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Dash_Into_Water_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Dash_Into_Water_Idle)
		.Init_End(this, &CSigrid_State::End_Dash_Into_Water_Idle)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"DASH_INTO_SURF")
		.Init_Start(this, &CSigrid_State::Start_Dash_Into_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Dash_Into_Surf)
		.Init_End(this, &CSigrid_State::End_Dash_Into_Surf)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"DASH_INTO_SURF_FAST")
		.Init_Start(this, &CSigrid_State::Start_Dash_Into_Surf_Fast)
		.Init_Tick(this, &CSigrid_State::Tick_Dash_Into_Surf_Fast)
		.Init_End(this, &CSigrid_State::End_Dash_Into_Surf_Fast)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Snap_Turn()
{
	m_pStateMachine->
		Add_State(L"SNAP_TURN_GROUND_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Snap_Turn_Ground_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Snap_Turn_Ground_Idle)
		.Init_End(this, &CSigrid_State::End_Snap_Turn_Ground_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyInput_Direction_Progress)

		.Add_State(L"SNAP_TURN_GROUND_RUN_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Snap_Turn_Ground_Run_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Snap_Turn_Ground_Run_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Snap_Turn_Ground_Run_Into_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"SNAP_TURN_GROUND_RUN")
		.Init_Start(this, &CSigrid_State::Start_Snap_Turn_Ground_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Snap_Turn_Ground_Run)
		.Init_End(this, &CSigrid_State::End_Snap_Turn_Ground_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SNAP_TURN_GROUND_RUN_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"SNAP_TURN_GROUND_BOOST_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Snap_Turn_Ground_Boost_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Snap_Turn_Ground_Boost_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Snap_Turn_Ground_Boost_Into_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"SNAP_TURN_GROUND_BOOST")
		.Init_Start(this, &CSigrid_State::Start_Snap_Turn_Ground_Boost)
		.Init_Tick(this, &CSigrid_State::Tick_Snap_Turn_Ground_Boost)
		.Init_End(this, &CSigrid_State::End_Snap_Turn_Ground_Boost)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"SANP_TURN_GROUND_BOOST_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GROUND_BOOST", this, &CSigrid_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Jump()
{
	m_pStateMachine->
		Add_State(L"JUMP")
		.Init_Start(this, &CSigrid_State::Start_Jump)
		.Init_Tick(this, &CSigrid_State::Tick_Jump)
		.Init_End(this, &CSigrid_State::End_Jump)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"AIR_ASCEND_90", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Jump_Double()
{
	m_pStateMachine->
		Add_State(L"JUMP_DOUBLE")
		.Init_Start(this, &CSigrid_State::Start_Jump_Double)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Double)
		.Init_End(this, &CSigrid_State::End_Jump_Double)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"AIR_DESCEND_60", this, &CSigrid_State::Animation_Progress_60_KeyInput_Direction)
		.Init_Changer(L"AIR_DESCEND_90", this, &CSigrid_State::Animation_Progress_60_KeyInput_None)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Jump_Charge()
{
	m_pStateMachine->
		Add_State(L"JUMP_CHARGING")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charging)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charging)
		.Init_End(this, &CSigrid_State::End_Jump_Charging)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"JUMP_CHARGING_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"JUMP_CHARGING_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_00_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_05_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_10_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_00)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_05)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_10)
		.Init_Changer(L"JUMP_CHARGED2_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_15_With_Dir)
		.Init_Changer(L"JUMP_CHARGED2_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_20_With_Dir)
		.Init_Changer(L"JUMP_CHARGED2_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_15)
		.Init_Changer(L"JUMP_CHARGED2_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_20)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"JUMP_CHARGING_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charging_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charging_Left)
		.Init_End(this, &CSigrid_State::End_Jump_Charging_Left)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"JUMP_CHARGING_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_00_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_05_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_10_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_00)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_05)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_10)
		.Init_Changer(L"JUMP_CHARGED2_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_15_With_Dir)
		.Init_Changer(L"JUMP_CHARGED2_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_20_With_Dir)
		.Init_Changer(L"JUMP_CHARGED2_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_15)
		.Init_Changer(L"JUMP_CHARGED2_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_20)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"JUMP_CHARGING_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charging_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charging_Right)
		.Init_End(this, &CSigrid_State::End_Jump_Charging_Right)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"JUMP_CHARGING_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_00_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_05_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_10_With_Dir)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_00)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_05)
		.Init_Changer(L"JUMP_CHARGED1_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_10)
		.Init_Changer(L"JUMP_CHARGED2_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_15_With_Dir)
		.Init_Changer(L"JUMP_CHARGED2_60", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_20_With_Dir)
		.Init_Changer(L"JUMP_CHARGED2_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_15)
		.Init_Changer(L"JUMP_CHARGED2_90", this, &CSigrid_State::KeyCharge_Space_Jump_Charge_20)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		/* 낮점 */
		.Add_State(L"JUMP_CHARGED1_60")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charged1_60)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charged1_60)
		.Init_End(this, &CSigrid_State::End_Jump_Charged1_60)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		//.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"JUMP_CHARGED1_90")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charged1_90)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charged1_90)
		.Init_End(this, &CSigrid_State::End_Jump_Charged1_90)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		//.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		/* 풀점 */
		.Add_State(L"JUMP_CHARGED2_60")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charged2_60)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charged2_60)
		.Init_End(this, &CSigrid_State::End_Jump_Charged2_60)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		//.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR_DESCEND_60", this, &CSigrid_State::Animation_Progress_60_KeyInput_Direction)
		.Init_Changer(L"AIR_DESCEND_90", this, &CSigrid_State::Animation_Progress_60_KeyInput_None)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"JUMP_CHARGED2_90")
		.Init_Start(this, &CSigrid_State::Start_Jump_Charged2_90)
		.Init_Tick(this, &CSigrid_State::Tick_Jump_Charged2_90)
		.Init_End(this, &CSigrid_State::End_Jump_Charged2_90)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		//.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"AIR_DESCEND_60", this, &CSigrid_State::Animation_Progress_60_KeyInput_Direction)
		.Init_Changer(L"AIR_DESCEND_90", this, &CSigrid_State::Animation_Progress_60_KeyInput_None)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Air()
{
	m_pStateMachine->
		Add_State(L"AIR")
		.Init_Start(this, &CSigrid_State::Start_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Air)
		.Init_End(this, &CSigrid_State::End_Air)
		.Init_Changer(L"AIR_ASCEND_60", this, &CSigrid_State::AirAscend60)
		.Init_Changer(L"AIR_ASCEND_90", this, &CSigrid_State::AirAscend90)
		.Init_Changer(L"AIR_DESCEND_60", this, &CSigrid_State::AirDescend_60)
		.Init_Changer(L"AIR_DESCEND_90", this, &CSigrid_State::AirDescend_90)
		.Init_Changer(L"AIR_DESCEND_FAST_90", this, &CSigrid_State::AirDescendFast)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"AIR_ASCEND_60")
		.Init_Start(this, &CSigrid_State::Start_Air_Ascend_60)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Ascend_60)
		.Init_End(this, &CSigrid_State::End_Air_Ascend_60)
		.Init_Changer(L"AIR", this, &CSigrid_State::Air0)
		.Init_Changer(L"AIR_ASCEND_90", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"AIR_ASCEND_90")
		.Init_Start(this, &CSigrid_State::Start_Air_Ascend_90)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Ascend_90)
		.Init_End(this, &CSigrid_State::End_Air_Ascend_90)
		.Init_Changer(L"AIR", this, &CSigrid_State::Air0)
		.Init_Changer(L"AIR_ASCEND_60", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"AIR_DESCEND_60")
		.Init_Start(this, &CSigrid_State::Start_Air_Descend_60)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Descend_60)
		.Init_End(this, &CSigrid_State::End_Air_Descend_60)
		.Init_Changer(L"AIR_DESCEND_90", this, &CSigrid_State::AirDescend_90)
		.Init_Changer(L"AIR_DESCEND_FAST_60", this, &CSigrid_State::AirDescendFast)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"AIR_DESCEND_90")
		.Init_Start(this, &CSigrid_State::Start_Air_Descend_90)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Descend_90)
		.Init_End(this, &CSigrid_State::End_Air_Descend_90)
		.Init_Changer(L"AIR_DESCEND_60", this, &CSigrid_State::AirDescend_60)
		.Init_Changer(L"AIR_DESCEND_FAST_90", this, &CSigrid_State::AirDescendFast)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"AIR_DESCEND_FAST_60")
		.Init_Start(this, &CSigrid_State::Start_Air_Descend_Fast_60)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Descend_Fast_60)
		.Init_End(this, &CSigrid_State::End_Air_Descend_Fast_60)
		.Init_Changer(L"AIR_DESCEND_60", this, &CSigrid_State::AirDescend_60)
		.Init_Changer(L"AIR_DESCEND_FAST_90", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
			.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Add_State(L"AIR_DESCEND_FAST_90")
		.Init_Start(this, &CSigrid_State::Start_Air_Descend_Fast_90)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Descend_Fast_90)
		.Init_End(this, &CSigrid_State::End_Air_Descend_Fast_90)
		.Init_Changer(L"AIR_DESCEND_90", this, &CSigrid_State::AirDescend_90)
		.Init_Changer(L"AIR_DESCEND_FAST_60", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyCharge_Space)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
			.Init_Changer(L"GRAPPLE_AIR_AIM", this, &CSigrid_State::MousePressing_RB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"COMBAT_GROUND_SLAM_INTRO", this, &CSigrid_State::KeyDown_LCTRL)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Air_Gliding()
{
	m_pStateMachine->
		Add_State(L"AIR_GLIDING")
		.Init_Start(this, &CSigrid_State::Start_Air_Gliding)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Gliding)
		.Init_End(this, &CSigrid_State::End_Air_Gliding)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR", this, &CSigrid_State::KeyUp_Space)
		.Init_Changer(L"AIR_GLIDING_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"AIR_GLIDING_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"AIR_GLIDING_RIGHT_TURN", this, &CSigrid_State::Turn_Back_Right)
		.Init_Changer(L"AIR_GLIDING_LEFT_TURN", this, &CSigrid_State::Turn_Back_Left)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"AIR_GLIDING_LEFT")
		.Init_Start(this, &CSigrid_State::Start_Air_Gliding_Left)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Gliding_Left)
		.Init_End(this, &CSigrid_State::End_Air_Gliding_Left)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR", this, &CSigrid_State::KeyUp_Space)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"AIR_GLIDING_RIGHT", this, &CSigrid_State::Camera_Angle_Turn_Right)
		.Init_Changer(L"AIR_GLIDING_RIGHT_TURN", this, &CSigrid_State::Turn_Back_Right)
		.Init_Changer(L"AIR_GLIDING_LEFT_TURN", this, &CSigrid_State::Turn_Back_Left)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"AIR_GLIDING_RIGHT")
		.Init_Start(this, &CSigrid_State::Start_Air_Gliding_Right)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Gliding_Right)
		.Init_End(this, &CSigrid_State::End_Air_Gliding_Right)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR", this, &CSigrid_State::KeyUp_Space)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::Camera_Angle_Coincide)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"AIR_GLIDING_LEFT", this, &CSigrid_State::Camera_Angle_Turn_Left)
		.Init_Changer(L"AIR_GLIDING_RIGHT_TURN", this, &CSigrid_State::Turn_Back_Right)
		.Init_Changer(L"AIR_GLIDING_LEFT_TURN", this, &CSigrid_State::Turn_Back_Left)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"AIR_GLIDING_RIGHT_TURN")
		.Init_Start(this, &CSigrid_State::Start_Air_Gliding_Right_Turn)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Gliding_Right_Turn)
		.Init_End(this, &CSigrid_State::End_Air_Gliding_Right_Turn)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR", this, &CSigrid_State::KeyUp_Space)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR_GLIDING_LEFT_TURN", this, &CSigrid_State::Turn_Back_Left)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"AIR_GLIDING_LEFT_TURN")
		.Init_Start(this, &CSigrid_State::Start_Air_Gliding_Left_Turn)
		.Init_Tick(this, &CSigrid_State::Tick_Air_Gliding_Left_Turn)
		.Init_End(this, &CSigrid_State::End_Air_Gliding_Left_Turn)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"AIR", this, &CSigrid_State::KeyUp_Space)
		.Init_Changer(L"AIR_GLIDING", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR_GLIDING_RIGHT_TURN", this, &CSigrid_State::Turn_Back_Right)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Landing()
{
	m_pStateMachine->
		Add_State(L"LANDING_GROUND_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Landing_Ground_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Landing_Ground_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Landing_Ground_Into_Idle)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Progress_50)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"LANDING_GROUND_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Landing_Ground_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Landing_Ground_Into_Run)
		.Init_End(this, &CSigrid_State::End_Landing_Ground_Into_Run)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Progress_30)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"LANDING_SLOPE")	/* Surf_Boost */
		.Init_Start(this, &CSigrid_State::Start_Landing_Slope)
		.Init_Tick(this, &CSigrid_State::Tick_Landing_Slope)
		.Init_End(this, &CSigrid_State::End_Landing_Slope)
		.Init_Changer(L"SURF_BOOST", this, &CSigrid_State::Animation_Progress_30)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"LANDING_SURF_FAST")
		.Init_Start(this, &CSigrid_State::Start_Landing_Surf_Fast)
		.Init_Tick(this, &CSigrid_State::Tick_Landing_Surf_Fast)
		.Init_End(this, &CSigrid_State::End_Landing_Surf_Fast)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Animation_Progress_30)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"LANDING_SURF")
		.Init_Start(this, &CSigrid_State::Start_Landing_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Landing_Surf)
		.Init_End(this, &CSigrid_State::End_Landing_Surf)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Progress_30)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)

		.Add_State(L"LANDING_WATER")
		.Init_Start(this, &CSigrid_State::Start_Landing_Water)
		.Init_Tick(this, &CSigrid_State::Tick_Landing_Water)
		.Init_End(this, &CSigrid_State::End_Landing_Water)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Animation_Progress_50)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space)
		.Init_Changer(L"DASH_INTO_WATER_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Combat_Combo()
{
	m_pStateMachine->
		Add_State(L"COMBAT_COMBO1_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo1_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo1_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Combat_Combo1_Into_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"COMBAT_COMBO2_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO1_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo1_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo1_Into_Run)
		.Init_End(this, &CSigrid_State::End_Combat_Combo1_Into_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"COMBAT_COMBO2_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)

		.Add_State(L"COMBAT_COMBO1_AIR")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo1_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo1_Air)
		.Init_End(this, &CSigrid_State::End_Combat_Combo1_Air)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"COMBAT_COMBO2_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space_Progress)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"COMBAT_COMBO1_INTO_SURF")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo1_Into_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo1_Into_Surf)
		.Init_End(this, &CSigrid_State::End_Combat_Combo1_Into_Surf)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO2_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO2_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo2_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo2_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Combat_Combo2_Into_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO2_INTO_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"COMBAT_COMBO3_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO2_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo2_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo2_Into_Run)
		.Init_End(this, &CSigrid_State::End_Combat_Combo2_Into_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO2_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"COMBAT_COMBO3_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)

		.Add_State(L"COMBAT_COMBO2_AIR")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo2_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo2_Air)
		.Init_End(this, &CSigrid_State::End_Combat_Combo2_Air)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"COMBAT_COMBO3_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space_Progress)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"COMBAT_COMBO2_INTO_SURF")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo2_Into_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo2_Into_Surf)
		.Init_End(this, &CSigrid_State::End_Combat_Combo2_Into_Surf)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO3_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO3_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo3_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo3_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Combat_Combo3_Into_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO3_INTO_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"COMBAT_COMBO4_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO3_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo3_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo3_Into_Run)
		.Init_End(this, &CSigrid_State::End_Combat_Combo3_Into_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO3_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"COMBAT_COMBO4_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)

		.Add_State(L"COMBAT_COMBO3_AIR")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo3_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo3_Air)
		.Init_End(this, &CSigrid_State::End_Combat_Combo3_Air)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"COMBAT_COMBO4_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space_Progress)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"COMBAT_COMBO3_INTO_SURF")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo3_Into_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo3_Into_Surf)
		.Init_End(this, &CSigrid_State::End_Combat_Combo3_Into_Surf)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO4_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO4_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo4_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo4_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Combat_Combo4_Into_Idle)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO4_INTO_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"COMBAT_COMBO1_INTO_IDLE", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_COMBO4_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo4_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo4_Into_Run)
		.Init_End(this, &CSigrid_State::End_Combat_Combo4_Into_Run)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO4_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"COMBAT_COMBO1_INTO_RUN", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)

		.Add_State(L"COMBAT_COMBO4_AIR")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo4_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo4_Air)
		.Init_End(this, &CSigrid_State::End_Combat_Combo4_Air)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"COMBAT_COMBO1_AIR", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space_Progress)
		//.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Collision_Ground)
		.Init_Changer(L"LANDING_GROUND_INTO_IDLE", this, &CSigrid_State::Collision_Ground_KeyInput_None)
		.Init_Changer(L"LANDING_GROUND_INTO_RUN", this, &CSigrid_State::Collision_Ground_KeyInput_Direction)
		.Init_Changer(L"LANDING_WATER", this, &CSigrid_State::Collision_Ocean_KeyInput_None)
		.Init_Changer(L"LANDING_SURF", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction)
		.Init_Changer(L"LANDING_SURF_FAST", this, &CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime)
		.Init_Changer(L"LANDING_SLOPE", this, &CSigrid_State::Collision_Ocean_Boost)

		.Add_State(L"COMBAT_COMBO4_INTO_SURF")
		.Init_Start(this, &CSigrid_State::Start_Combat_Combo4_Into_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Combo4_Into_Surf)
		.Init_End(this, &CSigrid_State::End_Combat_Combo4_Into_Surf)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_COMBO1_INTO_SURF", this, &CSigrid_State::MouseDown_LB)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Combat_Charge_Attack()
{
	m_pStateMachine->
		Add_State(L"COMBAT_CHARGE_ATTACK_INTO_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Combat_Charge_Attack_Into_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Charge_Attack_Into_Idle)
		.Init_End(this, &CSigrid_State::End_Combat_Charge_Attack_Into_Idle)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space_Progress)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_CHARGE_ATTACK_INTO_RUN")
		.Init_Start(this, &CSigrid_State::Start_Combat_Charge_Attack_Into_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Charge_Attack_Into_Run)
		.Init_End(this, &CSigrid_State::End_Combat_Charge_Attack_Into_Run)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space_Progress)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"AIR", this, &CSigrid_State::IsOnAir)

		.Add_State(L"COMBAT_CHARGE_ATTACK_INTO_WATER_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Combat_Charge_Attack_Into_Water_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Charge_Attack_Into_Water_Idle)
		.Init_End(this, &CSigrid_State::End_Combat_Charge_Attack_Into_Water_Idle)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_WATER_RUN", this, &CSigrid_State::KeyInput_Direction)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space_Progress)
		.Init_Changer(L"DASH_INTO_WATER_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"WATER_IDLE", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_CHARGE_ATTACK_INTO_WATER_RUN")
		.Init_Start(this, &CSigrid_State::Start_Combat_Charge_Attack_Into_Water_Run)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Charge_Attack_Into_Water_Run)
		.Init_End(this, &CSigrid_State::End_Combat_Charge_Attack_Into_Water_Run)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_WATER_IDLE", this, &CSigrid_State::KeyInput_None)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space_Progress)
		.Init_Changer(L"DASH_INTO_RUN", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"WATER_RUN", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_CHARGE_ATTACK_INTO_SURF")
		.Init_Start(this, &CSigrid_State::Start_Combat_Charge_Attack_Into_Surf)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Charge_Attack_Into_Surf)
		.Init_End(this, &CSigrid_State::End_Combat_Charge_Attack_Into_Surf)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST", this, &CSigrid_State::Check_SurfTime)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space_Progress)
		.Init_Changer(L"DASH_INTO_SURF", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"SURF", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_CHARGE_ATTACK_INTO_SURF_FAST")
		.Init_Start(this, &CSigrid_State::Start_Combat_Charge_Attack_Into_Surf_Fast)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Charge_Attack_Into_Surf_Fast)
		.Init_End(this, &CSigrid_State::End_Combat_Charge_Attack_Into_Surf_Fast)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"JUMP_CHARGING", this, &CSigrid_State::KeyInput_Space_Progress)
		.Init_Changer(L"DASH_INTO_SURF_FAST", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"SURF_FAST", this, &CSigrid_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Combat_Ground_Slam()
{
	m_pStateMachine->
		Add_State(L"COMBAT_GROUND_SLAM_INTRO")
		.Init_Start(this, &CSigrid_State::Start_Combat_Ground_Slam_Intro)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Ground_Slam_Intro)
		.Init_End(this, &CSigrid_State::End_Combat_Ground_Slam_Intro)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_GROUND_SLAM_FALL", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"COMBAT_GROUND_SLAM_FALL")
		.Init_Start(this, &CSigrid_State::Start_Combat_Ground_Slam_Fall)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Ground_Slam_Fall)
		.Init_End(this, &CSigrid_State::End_Combat_Ground_Slam_Fall)
		.Init_Changer(L"JUMP_DOUBLE", this, &CSigrid_State::KeyDoubleDown_Space)
		.Init_Changer(L"DASH_INTO_AIR", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"COMBAT_GROUND_SLAM_HIT", this, &CSigrid_State::Collision_Ground)

		.Add_State(L"COMBAT_GROUND_SLAM_HIT")
		.Init_Start(this, &CSigrid_State::Start_Combat_Ground_Slam_Hit)
		.Init_Tick(this, &CSigrid_State::Tick_Combat_Ground_Slam_Hit)
		.Init_End(this, &CSigrid_State::End_Combat_Ground_Slam_Hit)
		.Init_Changer(L"DAMAGE_HIT_IDLE", this, &CSigrid_State::Damaged)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"DASH_INTO_IDLE", this, &CSigrid_State::KeyDown_F)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)
		.Init_Changer(L"GROUND_RUN", this, &CSigrid_State::KeyInput_Direction_Progress)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Damaged()
{
	m_pStateMachine->Add_State(L"DAMAGE_DEATH_AIR_FALL_GROUND")
		.Init_Start(this, &CSigrid_State::Start_Damage_Death_Air_Fall_Ground)
		.Init_Tick(this, &CSigrid_State::Tick_Damage_Death_Air_Fall_Ground)
		.Init_End(this, &CSigrid_State::End_Damage_Death_Air_Fall_Ground)

		.Add_State(L"DAMAGE_DEATH_AIR_FALL_WATER")
		.Init_Start(this, &CSigrid_State::Start_Damage_Death_Air_Fall_Water)
		.Init_Tick(this, &CSigrid_State::Tick_Damage_Death_Air_Fall_Water)
		.Init_End(this, &CSigrid_State::End_Damage_Death_Air_Fall_Water)

		.Add_State(L"DAMAGE_DEATH_AIR_FALL")
		.Init_Start(this, &CSigrid_State::Start_Damage_Death_Air_Fall)
		.Init_Tick(this, &CSigrid_State::Tick_Damage_Death_Air_Fall)
		.Init_End(this, &CSigrid_State::End_Damage_Death_Air_Fall)

		.Add_State(L"DAMAGE_DEATH_AIR")
		.Init_Start(this, &CSigrid_State::Start_Damage_Death_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Damage_Death_Air)
		.Init_End(this, &CSigrid_State::End_Damage_Death_Air)

		.Add_State(L"DAMAGE_HIT_AIR")
		.Init_Start(this, &CSigrid_State::Start_Damage_Hit_Air)
		.Init_Tick(this, &CSigrid_State::Tick_Damage_Hit_Air)
		.Init_End(this, &CSigrid_State::End_Damage_Hit_Air)

		.Add_State(L"DAMAGE_HIT_IDLE")
		.Init_Start(this, &CSigrid_State::Start_Damage_Hit_Idle)
		.Init_Tick(this, &CSigrid_State::Tick_Damage_Hit_Idle)
		.Init_End(this, &CSigrid_State::End_Damage_Hit_Idle)
		.Init_Changer(L"GROUND_IDLE", this, &CSigrid_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Grapple_Ground()
{
	m_pStateMachine->Add_State(L"GRAPPLE_GROUND_FIRE")
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_FAST", this, &CSigrid_State::SnapGrappleFast)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_FAST_45", this, &CSigrid_State::SnapGrappleFast45)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_FAST_90", this, &CSigrid_State::SnapGrappleFast90)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_45", this, &CSigrid_State::SnapGrappleFastNeg45)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_90", this, &CSigrid_State::SnapGrappleFastNeg90)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_SLOW", this, &CSigrid_State::SnapGrappleSlow)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_SLOW_45", this, &CSigrid_State::SnapGrappleSlow45)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_SLOW_90", this, &CSigrid_State::SnapGrappleSlow90)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_45", this, &CSigrid_State::SnapGrappleSlowNeg45)
		.Init_Changer(L"GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_90", this, &CSigrid_State::SnapGrappleSlowNeg90)

		.Add_State(L"GRAPPLE_GROUND_FIRE_FAST")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Fast)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Fast)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Fast)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_GROUND_FIRE_FAST_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Fast_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Fast_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Fast_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_GROUND_FIRE_FAST_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Fast_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Fast_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Fast_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Fast_Negative_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Fast_Negative_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Fast_Negative_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Fast_Negative_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Fast_Negative_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Fast_Negative_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_GROUND_FIRE_SLOW")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Slow)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Slow)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Slow)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_GROUND_FIRE_SLOW_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Slow_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Slow_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Slow_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_GROUND_FIRE_SLOW_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Slow_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Slow_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Slow_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Slow_Negative_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Slow_Negative_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Slow_Negative_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Fire_Slow_Negative_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Fire_Slow_Negative_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Fire_Slow_Negative_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Grapple_Air()
{
	m_pStateMachine->Add_State(L"GRAPPLE_AIR_FIRE")
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_FAST", this, &CSigrid_State::SnapGrappleFast)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_FAST_45", this, &CSigrid_State::SnapGrappleFast45)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_FAST_90", this, &CSigrid_State::SnapGrappleFast90)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_FAST_NEGATIVE_45", this, &CSigrid_State::SnapGrappleFastNeg45)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_FAST_NEGATIVE_90", this, &CSigrid_State::SnapGrappleFastNeg90)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_SLOW", this, &CSigrid_State::SnapGrappleSlow)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_SLOW_45", this, &CSigrid_State::SnapGrappleSlow45)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_SLOW_90", this, &CSigrid_State::SnapGrappleSlow90)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_45", this, &CSigrid_State::SnapGrappleSlowNeg45)
		.Init_Changer(L"GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_90", this, &CSigrid_State::SnapGrappleSlowNeg90)

		.Add_State(L"GRAPPLE_AIR_FIRE_FAST")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Fast)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Fast)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Fast)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_AIR_FIRE_FAST_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Fast_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Fast_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Fast_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_AIR_FIRE_FAST_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Fast_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Fast_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Fast_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_AIR_FIRE_FAST_NEGATIVE_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Fast_Negative_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Fast_Negative_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Fast_Negative_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_AIR_FIRE_FAST_NEGATIVE_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Fast_Negative_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Fast_Negative_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Fast_Negative_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)
		.Init_Changer(L"AIR", this, &CSigrid_State::Grapple_Done)

		.Add_State(L"GRAPPLE_AIR_FIRE_SLOW")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Slow)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Slow)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Slow)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_AIR_FIRE_SLOW_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Slow_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Slow_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Slow_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_AIR_FIRE_SLOW_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Slow_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Slow_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Slow_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Slow_Negative_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Slow_Negative_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Slow_Negative_45)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Add_State(L"GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Fire_Slow_Negative_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Fire_Slow_Negative_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Fire_Slow_Negative_90)
		.Init_Changer(L"GRAPPLE_LAUNCH_FLIP", this, &CSigrid_State::ReadyLaunch)
		.Init_Changer(L"GRAPPLE_HANG_INTRO", this, &CSigrid_State::ReadyHang)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Grapple_Aim()
{
	m_pStateMachine->Add_State(L"GRAPPLE_GROUND_AIM")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Aim)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Aim)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Aim)

		.Add_State(L"GRAPPLE_GROUND_AIM_DOWN")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Aim_Down)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Aim_Down)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Aim_Down)

		.Add_State(L"GRAPPLE_GROUND_AIM_UP")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Ground_Aim_Up)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Ground_Aim_Up)
		.Init_End(this, &CSigrid_State::End_Grapple_Ground_Aim_Up)

		.Add_State(L"GRAPPLE_AIR_AIM")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Aim)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Aim)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Aim)
		.Init_Changer(L"AIR", this, &CSigrid_State::MouseUp_RB)

		.Add_State(L"GRAPPLE_AIR_AIM_DOWN")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Aim_Down)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Aim_Down)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Aim_Down)

		.Add_State(L"GRAPPLE_AIR_AIM_UP")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Air_Aim_Up)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Air_Aim_Up)
		.Init_End(this, &CSigrid_State::End_Grapple_Air_Aim_Up)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Grapple_Hang()
{
	m_pStateMachine->Add_State(L"GRAPPLE_HANG_INTRO")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Hang_Intro)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Hang_Intro)
		.Init_End(this, &CSigrid_State::End_Grapple_Hang_Intro)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)
		.Init_Changer(L"GRAPPLE_HANG", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"GRAPPLE_HANG")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Hang)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Hang)
		.Init_End(this, &CSigrid_State::End_Grapple_Hang)
		.Init_Changer(L"JUMP", this, &CSigrid_State::KeyDown_Space)
		.Init_Changer(L"GRAPPLE_AIR_FIRE", this, &CSigrid_State::SnapGrapple)

		.Finish_Setting();

	return S_OK;
}

HRESULT CSigrid_State::SetUp_State_Grapple_Launch()
{
	m_pStateMachine->Add_State(L"GRAPPLE_LAUNCH_FLIP")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch_Flip)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Launch_Flip)
		.Init_End(this, &CSigrid_State::End_Grapple_Launch_Flip)
		.Init_Changer(L"GRAPPLE_LAUNCH_READY", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"GRAPPLE_LAUNCH_READY")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch_Ready)
		.Init_Changer(L"GRAPPLE_LAUNCH", this, &CSigrid_State::GrappleLaunchAngle0)
		.Init_Changer(L"GRAPPLE_LAUNCH_45", this, &CSigrid_State::GrappleLaunchAngle45)
		.Init_Changer(L"GRAPPLE_LAUNCH_90", this, &CSigrid_State::GrappleLaunchAngle90)
		.Init_Changer(L"GRAPPLE_LAUNCH_NEGATIVE_45", this, &CSigrid_State::GrappleLaunchAngleNeg45)
		.Init_Changer(L"GRAPPLE_LAUNCH_NEGATIVE_90", this, &CSigrid_State::GrappleLaunchAngleNeg90)

		.Add_State(L"GRAPPLE_LAUNCH")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Launch)
		.Init_End(this, &CSigrid_State::End_Grapple_Launch)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"GRAPPLE_LAUNCH_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Launch_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Launch_45)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"GRAPPLE_LAUNCH_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Launch_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Launch_90)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"GRAPPLE_LAUNCH_NEGATIVE_45")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch_Negative_45)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Launch_Negative_45)
		.Init_End(this, &CSigrid_State::End_Grapple_Launch_Negative_45)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)

		.Add_State(L"GRAPPLE_LAUNCH_NEGATIVE_90")
		.Init_Start(this, &CSigrid_State::Start_Grapple_Launch_Negative_90)
		.Init_Tick(this, &CSigrid_State::Tick_Grapple_Launch_Negative_90)
		.Init_End(this, &CSigrid_State::End_Grapple_Launch_Negative_90)
		.Init_Changer(L"AIR", this, &CSigrid_State::Animation_Finish)

		.Finish_Setting();

	return S_OK;
}

void CSigrid_State::Start_Ground_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == SNAP_TURN_GROUND_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_bOnOcean = false;
	m_pPlayer->m_bBoost = false;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;

	m_pModelCom->Set_CurAnimationIndex(GROUND_IDLE);
}

void CSigrid_State::Start_Dash_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == DASH_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Movement_Dash();
		Play_Voice_Dash_Ground();
	}

	m_pPlayer->m_bDash = true;
	m_pModelCom->Set_CurAnimationIndex(DASH_INTO_IDLE);
	m_pModelCom->Reset_Animation();
	m_pPlayer->m_fCurDashTickCount = m_pPlayer->m_fMaxDashTickCount;
}

void CSigrid_State::Start_Ground_Run(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_bOnOcean = false;
	m_pPlayer->m_bBoost = false;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;

	m_pModelCom->Set_CurAnimationIndex(GROUND_RUN);
	if (m_pModelCom->Get_LastAnimationIndex() == GROUND_RUN_LEFT ||
		m_pModelCom->Get_LastAnimationIndex() == GROUND_RUN_RIGHT)
		m_pModelCom->Set_LerpTime(0.3f);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Ground_Run_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.3f);
	m_pModelCom->Set_CurAnimationIndex(GROUND_RUN_LEFT);
}

void CSigrid_State::Start_Ground_Run_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.3f);
	m_pModelCom->Set_CurAnimationIndex(GROUND_RUN_RIGHT);
}

void CSigrid_State::Start_Dash_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == DASH_INTO_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Movement_Dash();
		Play_Voice_Dash_Ground();
	}

	m_pPlayer->m_bDash = true;
	m_pModelCom->Set_CurAnimationIndex(DASH_INTO_RUN);
	m_pModelCom->Reset_Animation();
	m_pPlayer->m_fCurDashTickCount = m_pPlayer->m_fMaxDashTickCount;
}

void CSigrid_State::Start_Ground_Boost(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == SNAP_TURN_GROUND_BOOST)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_bOnOcean = false;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = true;

	m_pModelCom->Set_CurAnimationIndex(GROUND_BOOST);
}

void CSigrid_State::Start_Ground_Boost_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bBoost = true;
	m_pModelCom->Set_CurAnimationIndex(GROUND_BOOST_LEFT);
}

void CSigrid_State::Start_Ground_Boost_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bBoost = true;
	m_pModelCom->Set_CurAnimationIndex(GROUND_BOOST_RIGHT);
}

void CSigrid_State::Start_Ground_Boost_Stop(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GROUND_BOOST_STOP);
}

void CSigrid_State::Start_Water_Idle(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(WATER_IDLE);
}

void CSigrid_State::Start_Water_Run(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_dReduceSpeed = 0.01;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(GROUND_RUN);
	m_pModelCom->Set_CurAnimationSpeed(30.0);
}

void CSigrid_State::Start_Water_Run_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GROUND_RUN_LEFT);
	m_pModelCom->Set_CurAnimationSpeed(30.0);
}

void CSigrid_State::Start_Water_Run_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GROUND_RUN_RIGHT);
	m_pModelCom->Set_CurAnimationSpeed(30.0);
}

void CSigrid_State::Start_Water_Braking(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_dBrakeSpeed = 1.0;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(WATER_BRAKING);
}

void CSigrid_State::Start_Water_Diving_Exit(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(WATER_DIVING_EXIT);
}

void CSigrid_State::Start_Water_Diving(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(WATER_DIVING);
}

void CSigrid_State::Start_Surf_Intro(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(SURF_INTRO);
}

void CSigrid_State::Start_Surf(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_dReduceSpeed = 0.007;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(SURF);

	if (m_pModelCom->Get_LastAnimationIndex() != SURF_LEFT && m_pModelCom->Get_LastAnimationIndex() != SURF_RIGHT)
		m_pPlayer->m_dSurfTime = 0.0;

	if (m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);

	Play_SFX_Movement_Water_Surf_Slow_Start();
}

void CSigrid_State::Start_Surf_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(SURF_LEFT);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Surf_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(SURF_RIGHT);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Surf_Fast_Intro(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(SURF_FAST_INTRO);
}

void CSigrid_State::Start_Surf_Fast(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_dReduceSpeed = 0.004;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = false;

	m_pModelCom->Set_CurAnimationIndex(SURF_FAST);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);

	Play_SFX_Movement_Water_Surf_Fast_Start();
}

void CSigrid_State::Start_Surf_Fast_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(SURF_FAST_LEFT);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Surf_Fast_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(SURF_FAST_RIGHT);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Surf_Boost(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pPlayer->m_dReduceSpeed = 0.002;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pPlayer->m_bBoost = true;
	m_pModelCom->Set_CurAnimationIndex(SURF_BOOST);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_FAST|| m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);

	Play_SFX_Movement_Water_Surf_Boost_Start();
}

void CSigrid_State::Start_Surf_Boost_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bBoost = true;
	m_pModelCom->Set_CurAnimationIndex(SURF_BOOST_LEFT);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Surf_Boost_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bBoost = true;
	m_pModelCom->Set_CurAnimationIndex(SURF_BOOST_RIGHT);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);
}

void CSigrid_State::Start_Jump(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(JUMP);
	m_pModelCom->Reset_Animation();
	m_pPlayer->m_fCurJumpSpeed = m_pPlayer->m_fInitJumpSpeed;

	if (m_pPlayer->m_bOnOcean == false)
		Play_SFX_Movement_Ground_Jump();
	else
		Play_SFX_Movement_Water_Jump();

	Play_Voice_Jump();
}

void CSigrid_State::Start_Jump_Double(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_DOUBLE);
	m_pPlayer->m_fCurJumpSpeed = m_pPlayer->m_fInitJumpSpeed;

	Play_SFX_Movement_Double_Jump();

	Play_Voice_Double_Jump();
}

void CSigrid_State::Start_Jump_Charging(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGING);

	Play_SFX_Movement_ChargeJump_ChargeUp();
}

void CSigrid_State::Start_Jump_Charging_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGING_LEFT);

	if (m_pModelCom->Get_LastAnimationIndex() != JUMP_CHARGING && m_pModelCom->Get_LastAnimationIndex() != JUMP_CHARGING_RIGHT)
		Play_SFX_Movement_ChargeJump_ChargeUp();
}

void CSigrid_State::Start_Jump_Charging_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = false;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGING_RIGHT);

	if (m_pModelCom->Get_LastAnimationIndex() != JUMP_CHARGING && m_pModelCom->Get_LastAnimationIndex() != JUMP_CHARGING_LEFT)
		Play_SFX_Movement_ChargeJump_ChargeUp();
}

void CSigrid_State::Start_Jump_Charged1_60(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGED1_60);

	CGameInstance::GetInstance()->Stop_Sound(0);
	CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Movement_ChargeJump_Small();
	Play_Voice_Charge_Jump();
}

void CSigrid_State::Start_Jump_Charged1_90(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGED1_90);

	CGameInstance::GetInstance()->Stop_Sound(0);
	CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Movement_ChargeJump_Small();
	Play_Voice_Charge_Jump();
}

void CSigrid_State::Start_Jump_Charged2_60(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGED2_60);

	CGameInstance::GetInstance()->Stop_Sound(0);
	CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Movement_ChargeJump_Big();
	Play_Voice_Charge_Jump();
}

void CSigrid_State::Start_Jump_Charged2_90(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bDash = false;
	m_pModelCom->Set_CurAnimationIndex(JUMP_CHARGED2_90);

	CGameInstance::GetInstance()->Stop_Sound(0);
	CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Movement_ChargeJump_Big();
	Play_Voice_Charge_Jump();
}

void CSigrid_State::Start_Dash_Into_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == DASH_INTO_IDLE || m_pModelCom->Get_LastAnimationIndex() == DASH_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Movement_Dash();
		Play_Voice_Dash_Air();
	}

	m_pPlayer->m_bDash = true;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(DASH_INTO_AIR);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);
}

void CSigrid_State::Start_Dash_Into_Water_Idle(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bDash = true;
	m_pModelCom->Set_CurAnimationIndex(DASH_INTO_WATER_IDLE);
	
	Play_SFX_Movement_Dash();
	Play_Voice_Dash_Ground();
}

void CSigrid_State::Start_Dash_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bDash = true;
	m_pModelCom->Set_CurAnimationIndex(DASH_INTO_SURF);
	
	if (m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING_LEFT || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Movement_Dash();
	Play_Voice_Dash_Ground();
}

void CSigrid_State::Start_Dash_Into_Surf_Fast(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bOnOcean = true;
	m_pPlayer->m_bDash = true;
	m_pModelCom->Set_CurAnimationIndex(DASH_INTO_SURF_FAST);
	
	Play_SFX_Movement_Dash();
	Play_Voice_Dash_Ground();
}

void CSigrid_State::Start_Snap_Turn_Ground_Idle(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(SNAP_TURN_GROUND_IDLE);
}

void CSigrid_State::Start_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == SNAP_TURN_GROUND_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(SNAP_TURN_GROUND_RUN_INTO_IDLE);
}

void CSigrid_State::Start_Snap_Turn_Ground_Run(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(SNAP_TURN_GROUND_RUN);
}

void CSigrid_State::Start_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == SNAP_TURN_GROUND_BOOST)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(SNAP_TURN_GROUND_BOOST_INTO_IDLE);
}

void CSigrid_State::Start_Snap_Turn_Ground_Boost(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(SNAP_TURN_GROUND_BOOST);
}

void CSigrid_State::Start_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == JUMP || m_pModelCom->Get_LastAnimationIndex() == AIR_ASCEND_60 || m_pModelCom->Get_LastAnimationIndex() == AIR_ASCEND_90)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(AIR);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);
}

void CSigrid_State::Start_Air_Ascend_60(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == AIR || m_pModelCom->Get_LastAnimationIndex() == AIR_ASCEND_90)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(AIR_ASCEND_60);
}

void CSigrid_State::Start_Air_Ascend_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == JUMP || m_pModelCom->Get_LastAnimationIndex() == AIR || m_pModelCom->Get_LastAnimationIndex() == AIR_ASCEND_60)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(AIR_ASCEND_90);
}

void CSigrid_State::Start_Air_Descend_60(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == AIR || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_90 || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_FAST_60)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}

	else if (m_pModelCom->Get_LastAnimationIndex() == JUMP_DOUBLE || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGED2_60 || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGED2_90)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;

	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(AIR_DESCEND_60);
}

void CSigrid_State::Start_Air_Descend_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == AIR || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_60 || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_FAST_90)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}

	else if (m_pModelCom->Get_LastAnimationIndex() == JUMP_DOUBLE || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGED2_60 || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGED2_90)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;

	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(AIR_DESCEND_90);
}

void CSigrid_State::Start_Air_Descend_Fast_60(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_60 || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_FAST_90)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(AIR_DESCEND_FAST_60);
}

void CSigrid_State::Start_Air_Descend_Fast_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_LastAnimationIndex() == AIR || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_90 || m_pModelCom->Get_LastAnimationIndex() == AIR_DESCEND_FAST_60)
	{
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
		m_pModelCom->Set_LerpTime(2.f);
	}
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(AIR_DESCEND_FAST_90);
}

void CSigrid_State::Start_Air_Gliding(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bDoubleJump = true;
	m_pPlayer->m_fGravity /= 20.f;
	m_pPlayer->m_fCurJumpSpeed = 0.f;

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		m_pModelCom->Set_LerpTime(0.01f);
	else
	{
		m_pModelCom->Set_LerpTime(0.2f);

		if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT && m_pModelCom->Get_LastAnimationIndex() != AIR_GLIDING_RIGHT)
			Play_SFX_Movement_Glide_Fly_Start();
	}

	m_pModelCom->Set_CurAnimationIndex(AIR_GLIDING);
}

void CSigrid_State::Start_Air_Gliding_Left(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bDoubleJump = true;
	m_pPlayer->m_fGravity /= 20.f;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(AIR_GLIDING_LEFT);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Start_Air_Gliding_Right(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bDoubleJump = true;
	m_pPlayer->m_fGravity /= 20.f;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(AIR_GLIDING_RIGHT);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Start_Air_Gliding_Left_Turn(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bDoubleJump = true;
	m_pPlayer->m_fGravity /= 20.f;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_LerpTime(0.1f);
	m_pModelCom->Set_CurAnimationIndex(AIR_GLIDING_LEFT_TURN);

	Play_SFX_Movement_Glide_Fly_Start();
}

void CSigrid_State::Start_Air_Gliding_Right_Turn(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_bDoubleJump = true;
	m_pPlayer->m_fGravity /= 20.f;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_LerpTime(0.1f);
	m_pModelCom->Set_CurAnimationIndex(AIR_GLIDING_RIGHT_TURN);

	Play_SFX_Movement_Glide_Fly_Start();
}

void CSigrid_State::Start_Landing_Ground_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(LANDING_GROUND_INTO_IDLE);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	Play_SFX_Movement_Ground_Land();

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_Voice_Land_Hard();
	else
		Play_Voice_Land_Soft();
}

void CSigrid_State::Start_Landing_Ground_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(LANDING_GROUND_INTO_RUN);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	Play_SFX_Movement_Ground_Land();

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_Voice_Land_Hard();
	else
		Play_Voice_Land_Soft();
}

void CSigrid_State::Start_Landing_Slope(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(LANDING_SLOPE);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_SFX_Movement_Water_Land_Hard();
	else
		Play_SFX_Movement_Water_Land();

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_Voice_Land_Hard();
	else
		Play_Voice_Land_Soft();
}

void CSigrid_State::Start_Landing_Surf_Fast(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(LANDING_SURF_FAST);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_SFX_Movement_Water_Land_Hard();
	else
		Play_SFX_Movement_Water_Land();

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_Voice_Land_Hard();
	else
		Play_Voice_Land_Soft();
}

void CSigrid_State::Start_Landing_Surf(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(LANDING_SURF);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_SFX_Movement_Water_Land_Hard();
	else
		Play_SFX_Movement_Water_Land();

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_Voice_Land_Hard();
	else
		Play_Voice_Land_Soft();
}

void CSigrid_State::Start_Landing_Water(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(LANDING_WATER);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT_TURN || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_SFX_Movement_Water_Land_Hard();
	else
		Play_SFX_Movement_Water_Land();

	if (m_pPlayer->m_fCurJumpSpeed < -0.5f)
		Play_Voice_Land_Hard();
	else
		Play_Voice_Land_Soft();
}

void CSigrid_State::Start_Combat_Combo1_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO1_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_1_3();
		Play_Voice_Attack_Combo_1_3();
	}

	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO1_INTO_IDLE);
}

void CSigrid_State::Start_Combat_Combo1_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO1_INTO_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_1_3();
		Play_Voice_Attack_Combo_1_3();
	}

	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO1_INTO_RUN);
}

void CSigrid_State::Start_Combat_Combo1_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO1_INTO_SURF);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);

	if (m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING_LEFT || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Attack_Combo_1_3();
	Play_Voice_Attack_Combo_1_3();
}

void CSigrid_State::Start_Combat_Combo1_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO1_AIR);

	if (m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_LastAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);

	Play_SFX_Attack_Combo_1_3();
	Play_Voice_Attack_Combo_1_3();
}

void CSigrid_State::Start_Combat_Combo2_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO2_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_2_4();
		Play_Voice_Attack_Combo_2_4();
	}

	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO2_INTO_IDLE);
}

void CSigrid_State::Start_Combat_Combo2_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO2_INTO_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_2_4();
		Play_Voice_Attack_Combo_2_4();
	}

	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO2_INTO_RUN);
}

void CSigrid_State::Start_Combat_Combo2_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO2_INTO_SURF);
	Play_SFX_Attack_Combo_2_4();
	Play_Voice_Attack_Combo_2_4();
}

void CSigrid_State::Start_Combat_Combo2_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO2_AIR);
	Play_SFX_Attack_Combo_2_4();
	Play_Voice_Attack_Combo_2_4();
}

void CSigrid_State::Start_Combat_Combo3_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO3_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_1_3();
		Play_Voice_Attack_Combo_1_3();
	}
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO3_INTO_IDLE);
}

void CSigrid_State::Start_Combat_Combo3_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO3_INTO_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_1_3();
		Play_Voice_Attack_Combo_1_3();
	}
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO3_INTO_RUN);
}

void CSigrid_State::Start_Combat_Combo3_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO3_INTO_SURF);
	Play_SFX_Attack_Combo_1_3();
	Play_Voice_Attack_Combo_1_3();
}

void CSigrid_State::Start_Combat_Combo3_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO3_AIR);
	Play_SFX_Attack_Combo_1_3();
	Play_Voice_Attack_Combo_1_3();
}

void CSigrid_State::Start_Combat_Combo4_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO4_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_2_4();
		Play_Voice_Attack_Combo_2_4();
	}
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO4_INTO_IDLE);
}

void CSigrid_State::Start_Combat_Combo4_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_COMBO4_INTO_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
	{
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
		Play_SFX_Attack_Combo_2_4();
		Play_Voice_Attack_Combo_2_4();
	}
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO4_INTO_RUN);
}

void CSigrid_State::Start_Combat_Combo4_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO4_INTO_SURF);
	Play_SFX_Attack_Combo_2_4();
	Play_Voice_Attack_Combo_2_4();
}

void CSigrid_State::Start_Combat_Combo4_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = false;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_COMBO4_AIR);
	Play_SFX_Attack_Combo_2_4();
	Play_Voice_Attack_Combo_2_4();
}

void CSigrid_State::Start_Combat_Charge_Attack_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_CHARGE_ATTACK_INTO_RUN)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(COMBAT_CHARGE_ATTACK_INTO_IDLE);
}

void CSigrid_State::Start_Combat_Charge_Attack_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_CHARGE_ATTACK_INTO_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(COMBAT_CHARGE_ATTACK_INTO_RUN);
}

void CSigrid_State::Start_Combat_Charge_Attack_Into_Water_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_CHARGE_ATTACK_INTO_RUN && m_pPlayer->m_bOnOcean == true)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(COMBAT_CHARGE_ATTACK_INTO_WATER_IDLE);
}

void CSigrid_State::Start_Combat_Charge_Attack_Into_Water_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_CHARGE_ATTACK_INTO_WATER_IDLE)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(COMBAT_CHARGE_ATTACK_INTO_RUN);
}

void CSigrid_State::Start_Combat_Charge_Attack_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(COMBAT_CHARGE_ATTACK_INTO_SURF);
}

void CSigrid_State::Start_Combat_Charge_Attack_Into_Surf_Fast(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	if (m_pModelCom->Get_LastAnimationIndex() == COMBAT_CHARGE_ATTACK_INTO_SURF)
		m_pPlayer->m_eLerpType = CModel::LERP_CONTINUE;
	else
		m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;

	m_pModelCom->Set_CurAnimationIndex(COMBAT_CHARGE_ATTACK_INTO_SURF_FAST);
}

void CSigrid_State::Start_Combat_Ground_Slam_Intro(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pPlayer->m_fGravity *= 10.f;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_GROUND_SLAM_INTRO);

	Play_SFX_Attack_GroundSlam_Intro();
	Play_Voice_Attack_Ground_Slam_Intro();
}

void CSigrid_State::Start_Combat_Ground_Slam_Fall(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(COMBAT_GROUND_SLAM_FALL);
}

void CSigrid_State::Start_Combat_Ground_Slam_Hit(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = true;
	m_pPlayer->m_bSpecialAttack = true;

	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_LerpTime(0.05f);
	m_pModelCom->Set_CurAnimationIndex(COMBAT_GROUND_SLAM_HIT);

	Play_SFX_Attack_GroundSlam_Hit();
	Play_Voice_Attack_Ground_Slam();
}

void CSigrid_State::Start_Damage_Death_Air_Fall_Ground(_double dTimeDelta)
{
}

void CSigrid_State::Start_Damage_Death_Air_Fall_Water(_double dTimeDelta)
{
}

void CSigrid_State::Start_Damage_Death_Air_Fall(_double dTimeDelta)
{
}

void CSigrid_State::Start_Damage_Death_Air(_double dTimeDelta)
{
}

void CSigrid_State::Start_Damage_Hit_Air(_double dTimeDelta)
{
}

void CSigrid_State::Start_Damage_Hit_Idle(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(DAMAGE_HIT_IDLE);

	if (m_pModelCom->Get_LastAnimationIndex() == SURF || m_pModelCom->Get_LastAnimationIndex() == SURF_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_FAST || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_LastAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);

	if (m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING_LEFT || m_pModelCom->Get_LastAnimationIndex() == JUMP_CHARGING_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(6);

	Play_SFX_Damaged();
	Play_Voice_Damaged();
}

void CSigrid_State::Start_Grapple_Ground_Fire_Fast(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_FAST);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Ground_Fire_Fast_45(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_FAST_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Ground_Fire_Fast_90(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_FAST_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Ground_Fire_Fast_Negative_45(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Ground_Fire_Fast_Negative_90(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Ground_Fire_Slow(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_SLOW);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Ground_Fire_Slow_45(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_SLOW_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Ground_Fire_Slow_90(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_SLOW_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Ground_Fire_Slow_Negative_45(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Ground_Fire_Slow_Negative_90(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Air_Fire_Fast(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_FAST);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Air_Fire_Fast_45(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_FAST_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Air_Fire_Fast_90(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_FAST_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Air_Fire_Fast_Negative_45(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_FAST_NEGATIVE_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Air_Fire_Fast_Negative_90(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_FAST_NEGATIVE_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);

	Play_SFX_HookShot_AimIn();
	Play_SFX_HookShot_In_Swish();
}

void CSigrid_State::Start_Grapple_Air_Fire_Slow(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_SLOW);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Air_Fire_Slow_45(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_SLOW_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Air_Fire_Slow_90(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_SLOW_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Air_Fire_Slow_Negative_45(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_45);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Air_Fire_Slow_Negative_90(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_90);
	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos);
}

void CSigrid_State::Start_Grapple_Ground_Aim(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_AIM);
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 0.2);
	m_pPlayer->m_dTimeScale = 0.2;
	m_pPlayer->m_pCamera->Set_TimeScale(0.2);
}

void CSigrid_State::Start_Grapple_Ground_Aim_Down(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_AIM_DOWN);
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 0.2);
	m_pPlayer->m_dTimeScale = 0.2;
	m_pPlayer->m_pCamera->Set_TimeScale(0.2);
}

void CSigrid_State::Start_Grapple_Ground_Aim_Up(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_GROUND_AIM_UP);
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 0.2);
	m_pPlayer->m_dTimeScale = 0.2;
	m_pPlayer->m_pCamera->Set_TimeScale(0.2);
}

void CSigrid_State::Start_Grapple_Air_Aim(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fGravity /= 40.f;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_AIM);
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 0.2);
	m_pPlayer->m_dTimeScale = 0.2;
	m_pPlayer->m_pCamera->Set_TimeScale(0.2);
}

void CSigrid_State::Start_Grapple_Air_Aim_Down(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fGravity /= 40.f;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_AIM_DOWN);
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 0.2);
	m_pPlayer->m_dTimeScale = 0.2;
	m_pPlayer->m_pCamera->Set_TimeScale(0.2);
}

void CSigrid_State::Start_Grapple_Air_Aim_Up(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pPlayer->m_fGravity /= 40.f;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_AIR_AIM_UP);
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 0.2);
	m_pPlayer->m_dTimeScale = 0.2;
	m_pPlayer->m_pCamera->Set_TimeScale(0.2);
}

void CSigrid_State::Start_Grapple_Hang_Intro(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bSnapGrappleFast = false;
	m_pPlayer->m_bSnapGrappleSlow = false;
	m_pPlayer->m_bGrappleLauncher = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_HANG_INTRO);

	Play_Voice_Grapple_Arrive_Hand();
}

void CSigrid_State::Start_Grapple_Hang(_double dTimeDelta)
{
	m_pPlayer->m_bDoubleJump = false;
	m_pPlayer->m_bSnapGrappleFast = false;
	m_pPlayer->m_bSnapGrappleSlow = false;
	m_pPlayer->m_bGrappleLauncher = false;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_HANG);
}

void CSigrid_State::Start_Grapple_Launch_Flip(_double dTimeDelta)
{
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_LAUNCH_FLIP);

	_vector	vTargetLook = XMVectorSetY(m_pPlayer->m_vSnapGrappleLook, 0.f);
	_vector	vPlayerLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);

	_vector	vDir = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - m_pPlayer->m_vSnapGrapplePos, 0.f);

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(m_pPlayer->m_vSnapGrappleRight)));

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vTargetLook), XMVector3Normalize(vPlayerLook))));

	if (fDir < 0.f)
		fAngle *= -1.f;

	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
}

void CSigrid_State::Start_Grapple_Launch_Ready(_double dTimeDelta)
{
	m_pPlayer->m_bSnapGrappleFast = false;
	m_pPlayer->m_bSnapGrappleSlow = false;

	Play_SFX_HookShot_Out_Launch();
	Play_Voice_Grapple_Launch();
}

void CSigrid_State::Start_Grapple_Launch(_double dTimeDelta)
{
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_LAUNCH);

	_vector	vTargetLook = XMVectorSetY(m_pPlayer->m_vSnapGrappleLook, 0.f);
	_vector	vPlayerLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);

	_vector	vDir = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - m_pPlayer->m_vSnapGrapplePos, 0.f);

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(m_pPlayer->m_vSnapGrappleRight)));

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vTargetLook), XMVector3Normalize(vPlayerLook))));

	if (fDir < 0.f)
		fAngle *= -1.f;

	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos + XMVector3Normalize(m_pPlayer->m_vSnapGrappleLook));
}

void CSigrid_State::Start_Grapple_Launch_45(_double dTimeDelta)
{
	m_pPlayer->m_fCurJumpSpeed = 1.f;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_LAUNCH_45);

	_vector	vTargetLook = XMVectorSetY(m_pPlayer->m_vSnapGrappleLook, 0.f);
	_vector	vPlayerLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);

	_vector	vDir = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - m_pPlayer->m_vSnapGrapplePos, 0.f);

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(m_pPlayer->m_vSnapGrappleRight)));

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vTargetLook), XMVector3Normalize(vPlayerLook))));

	if (fDir < 0.f)
		fAngle *= -1.f;

	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos + XMVector3Normalize(m_pPlayer->m_vSnapGrappleLook));
}

void CSigrid_State::Start_Grapple_Launch_90(_double dTimeDelta)
{
	m_pPlayer->m_fCurJumpSpeed = 1.2f;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_LAUNCH_90);

	_vector	vTargetLook = XMVectorSetY(m_pPlayer->m_vSnapGrappleLook, 0.f);
	_vector	vPlayerLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);

	_vector	vDir = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - m_pPlayer->m_vSnapGrapplePos, 0.f);

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(m_pPlayer->m_vSnapGrappleRight)));

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vTargetLook), XMVector3Normalize(vPlayerLook))));

	if (fDir < 0.f)
		fAngle *= -1.f;

	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos + XMVector3Normalize(m_pPlayer->m_vSnapGrappleLook));
}

void CSigrid_State::Start_Grapple_Launch_Negative_45(_double dTimeDelta)
{
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_LAUNCH_NEGATIVE_45);

	_vector	vTargetLook = XMVectorSetY(m_pPlayer->m_vSnapGrappleLook, 0.f);
	_vector	vPlayerLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);

	_vector	vDir = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - m_pPlayer->m_vSnapGrapplePos, 0.f);

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(m_pPlayer->m_vSnapGrappleRight)));

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vTargetLook), XMVector3Normalize(vPlayerLook))));

	if (fDir < 0.f)
		fAngle *= -1.f;

	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos + XMVector3Normalize(m_pPlayer->m_vSnapGrappleLook));
}

void CSigrid_State::Start_Grapple_Launch_Negative_90(_double dTimeDelta)
{
	m_pPlayer->m_fCurJumpSpeed = 0.f;
	m_pPlayer->m_eLerpType = CModel::LERP_BEGIN;
	m_pModelCom->Set_CurAnimationIndex(GRAPPLE_LAUNCH_NEGATIVE_90);

	_vector	vTargetLook = XMVectorSetY(m_pPlayer->m_vSnapGrappleLook, 0.f);
	_vector	vPlayerLook = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f);

	_vector	vDir = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - m_pPlayer->m_vSnapGrapplePos, 0.f);

	_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(m_pPlayer->m_vSnapGrappleRight)));

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vTargetLook), XMVector3Normalize(vPlayerLook))));

	if (fDir < 0.f)
		fAngle *= -1.f;

	m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);

	m_pTransformCom->LookAt_NoUpDown(m_pPlayer->m_vSnapGrapplePos + XMVector3Normalize(m_pPlayer->m_vSnapGrappleLook));
}

void CSigrid_State::Tick_Ground_Idle(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Dash_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.05f && m_pModelCom->Get_AnimationProgress() < 0.45f)
	{
		m_pTransformCom->Dash(dTimeDelta, m_pPlayer->m_fFriction, m_pPlayer->m_fCurDashTickCount, m_pCamera->Get_WorldMatrix(), m_eDir, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		m_dEffectCloneTimer += dTimeDelta;

		if (m_dEffectCloneTimer > 0.025)
		{
			m_pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Sigrid_Dash", m_pPlayer->Get_WorldMatrix());
			m_dEffectCloneTimer = 0.0;
		}
	}
}

void CSigrid_State::Tick_Ground_Run(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.1f)
	{
		_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
		_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

		if (XMVectorGetY(matLeftFoot.r[3]) - 0.0195f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.002f < 0.f)
			Play_SFX_Movement_Ground_Footstep();
	}
}

void CSigrid_State::Tick_Ground_Run_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.1f)
	{
		_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
		_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

		if (XMVectorGetY(matLeftFoot.r[3]) - 0.f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.f < 0.f)
			Play_SFX_Movement_Ground_Footstep();
	}
}

void CSigrid_State::Tick_Ground_Run_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.1f)
	{
		_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
		_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

		if (XMVectorGetY(matLeftFoot.r[3]) - 0.f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.f < 0.f)
			Play_SFX_Movement_Ground_Footstep();
	}
}

void CSigrid_State::Tick_Dash_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.05f && m_pModelCom->Get_AnimationProgress() < 0.45f)
	{
		m_pTransformCom->Dash(dTimeDelta, m_pPlayer->m_fFriction, m_pPlayer->m_fCurDashTickCount, m_pCamera->Get_WorldMatrix(), m_eDir, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		m_dEffectCloneTimer += dTimeDelta;

		if (m_dEffectCloneTimer > 0.025)
		{
			m_pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Sigrid_Dash", m_pPlayer->Get_WorldMatrix());
			m_dEffectCloneTimer = 0.0;
		}
	}

	else if (m_pModelCom->Get_AnimationProgress() > 0.45f)
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Ground_Boost(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.05f)
	{
		_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
		_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

		if (XMVectorGetY(matLeftFoot.r[3]) - 0.f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.f < 0.f)
			Play_SFX_Movement_Ground_Footstep();
	}

	Play_Voice_Running();
}

void CSigrid_State::Tick_Ground_Boost_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.05f)
	{
		_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
		_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

		if (XMVectorGetY(matLeftFoot.r[3]) - 0.002f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.f < 0.f)
			Play_SFX_Movement_Ground_Footstep();
	}

	Play_Voice_Running();
}

void CSigrid_State::Tick_Ground_Boost_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.05f)
	{
		_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
		_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

		if (XMVectorGetY(matLeftFoot.r[3]) - 0.f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.f < 0.f)
			Play_SFX_Movement_Ground_Footstep();
	}

	Play_Voice_Running();
}

void CSigrid_State::Tick_Ground_Boost_Stop(_double dTimeDelta) 
{
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Water_Idle(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Water_Run(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
	_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

	if (XMVectorGetY(matLeftFoot.r[3]) - 0.02f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.002f < 0.f)
		Play_SFX_Movement_Water_Footstep();
}

void CSigrid_State::Tick_Water_Run_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
	_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

	if (XMVectorGetY(matLeftFoot.r[3]) - 0.02f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.002f < 0.f)
		Play_SFX_Movement_Water_Footstep();
}

void CSigrid_State::Tick_Water_Run_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	_matrix	matLeftFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Leg Toe");
	_matrix	matRightFoot = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Leg Toe");

	if (XMVectorGetY(matLeftFoot.r[3]) - 0.02f < 0.f || XMVectorGetY(matRightFoot.r[3]) - 0.01f < 0.f)
		Play_SFX_Movement_Water_Footstep();
}

void CSigrid_State::Tick_Water_Braking(_double dTimeDelta)
{
	if (m_pPlayer->m_dBrakeSpeed > 0.005)
	{
		Move(dTimeDelta * m_pPlayer->m_dBrakeSpeed, CTransform::DIR_LOOK);
		m_pPlayer->m_dBrakeSpeed -= m_pPlayer->m_dReduceSpeed;
	}
}

void CSigrid_State::Tick_Water_Diving_Exit(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Water_Diving(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Surf_Intro(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Surf(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_BaseLoop.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_BaseLoop.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_BaseLoop.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Fast_Intro(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Surf_Fast(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_Lean.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Fast_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_Lean.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Fast_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_Lean.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Boost(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanLoop.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Boost_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanLoop.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Surf_Boost_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanLoop.wav", g_fSFXVolume, false, false, 0);
}

void CSigrid_State::Tick_Snap_Turn_Ground_Idle(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Snap_Turn_Ground_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.35f)
		Move(dTimeDelta, m_eDir, CSigrid_State::MOVE_TURNBACK);
	else
		Move(dTimeDelta, m_eDir, CSigrid_State::MOVE_DYNAMIC);
}

void CSigrid_State::Tick_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.35f)
		Move(dTimeDelta, m_eDir, CSigrid_State::MOVE_TURNBACK);
	else
		Move(dTimeDelta, m_eDir, CSigrid_State::MOVE_DYNAMIC);
}

void CSigrid_State::Tick_Snap_Turn_Ground_Boost(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.35f)
		Move(dTimeDelta, m_eDir, CSigrid_State::MOVE_TURNBACK);
	else
		Move(dTimeDelta, m_eDir, CSigrid_State::MOVE_DYNAMIC);
}

void CSigrid_State::Tick_Jump(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Jump_Double(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;
	m_pPlayer->m_bDoubleJump = true;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Jump_Charging(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Loop.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Tick_Jump_Charging_Left(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Loop.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Tick_Jump_Charging_Right(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Loop.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Tick_Jump_Charged1_60(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Jump_Charged1_90(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);

	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta * 0.3f, m_eDir);
}

void CSigrid_State::Tick_Jump_Charged2_60(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Jump_Charged2_90(_double dTimeDelta)
{
	m_pPlayer->m_bJump = true;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);    

	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta * 0.3f, m_eDir);
}

void CSigrid_State::Tick_Dash_Into_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.05f && m_pModelCom->Get_AnimationProgress() < 0.45f)
	{
		m_pTransformCom->Dash(dTimeDelta, m_pPlayer->m_fFriction, m_pPlayer->m_fCurDashTickCount, m_pCamera->Get_WorldMatrix(), m_eDir, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		m_dEffectCloneTimer += dTimeDelta;

		if (m_dEffectCloneTimer > 0.025)
		{
			m_pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Sigrid_Dash", m_pPlayer->Get_WorldMatrix());
			m_dEffectCloneTimer = 0.0;
		}
	}

	else if (m_pModelCom->Get_AnimationProgress() > 0.45f)
	{
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
		Move(dTimeDelta, m_eDir);
	}
}

void CSigrid_State::Tick_Dash_Into_Water_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.05f && m_pModelCom->Get_AnimationProgress() < 0.45f)
	{
		m_pTransformCom->Dash(dTimeDelta, m_pPlayer->m_fFriction, m_pPlayer->m_fCurDashTickCount, m_pCamera->Get_WorldMatrix(), m_eDir, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		m_dEffectCloneTimer += dTimeDelta;

		if (m_dEffectCloneTimer > 0.025)
		{
			m_pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Sigrid_Dash", m_pPlayer->Get_WorldMatrix());
			m_dEffectCloneTimer = 0.0;
		}
	}
}

void CSigrid_State::Tick_Dash_Into_Surf(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.05f && m_pModelCom->Get_AnimationProgress() < 0.45f)
	{
		m_pTransformCom->Dash(dTimeDelta, m_pPlayer->m_fFriction, m_pPlayer->m_fCurDashTickCount, m_pCamera->Get_WorldMatrix(), m_eDir, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		m_dEffectCloneTimer += dTimeDelta;

		if (m_dEffectCloneTimer > 0.025)
		{
			m_pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Sigrid_Dash", m_pPlayer->Get_WorldMatrix());
			m_dEffectCloneTimer = 0.0;
		}
	}
	else if (m_pModelCom->Get_AnimationProgress() > 0.45f)
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Dash_Into_Surf_Fast(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.05f && m_pModelCom->Get_AnimationProgress() < 0.45f)
	{
		m_pTransformCom->Dash(dTimeDelta, m_pPlayer->m_fFriction, m_pPlayer->m_fCurDashTickCount, m_pCamera->Get_WorldMatrix(), m_eDir, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		m_dEffectCloneTimer += dTimeDelta;

		if (m_dEffectCloneTimer > 0.025)
		{
			m_pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Sigrid_Dash", m_pPlayer->Get_WorldMatrix());
			m_dEffectCloneTimer = 0.0;
		}
	}
	else if (m_pModelCom->Get_AnimationProgress() > 0.45f)
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air(_double dTimeDelta)
{
	if (m_pPlayer->m_dPauseTime < 1.0)
		return;

	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Ascend_60(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Ascend_90(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Descend_60(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Descend_90(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Descend_Fast_60(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Descend_Fast_90(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Air_Gliding(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);

	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Tick_Air_Gliding_Left(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	
	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Tick_Air_Gliding_Right(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	
	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Tick_Air_Gliding_Left_Turn(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	
	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Tick_Air_Gliding_Right_Turn(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	
	if (m_eDir != CTransform::DIR_END)
		Move(dTimeDelta, m_eDir);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Loop.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Tick_Landing_Ground_Into_Idle(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Landing_Ground_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.1f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Move(dTimeDelta * 0.7, m_eDir);
	else
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Landing_Slope(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.1f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Move(dTimeDelta * 0.7, m_eDir);
	else
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Landing_Surf_Fast(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.1f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Move(dTimeDelta * 0.7, m_eDir);
	else
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Landing_Surf(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.1f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Move(dTimeDelta * 0.7, m_eDir);
	else
		Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Landing_Water(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Combat_Combo1_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo1_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
		Move(dTimeDelta, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo1_Into_Surf(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo1_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
	{
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
		Move(dTimeDelta, m_eDir);
	}

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo2_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo2_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
		Move(dTimeDelta, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo2_Into_Surf(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo2_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
	{
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
		Move(dTimeDelta, m_eDir);
	}

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo3_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo3_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
		Move(dTimeDelta, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo3_Into_Surf(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo3_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
	{
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
		Move(dTimeDelta, m_eDir);
	}

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo4_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo4_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
		Move(dTimeDelta, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo4_Into_Surf(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Combo4_Air(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.05f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.5f)
	{
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
		Move(dTimeDelta, m_eDir);
	}

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Charge_Attack_Into_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.2f)
		Move(dTimeDelta * 0.5f, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.10f && m_pModelCom->Get_AnimationProgress() < 0.15f)
		Play_Voice_Charge_Attack();
	if (m_pModelCom->Get_AnimationProgress() > 0.15f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Play_SFX_Attack_Charge_Start();
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.25f)
		Play_SFX_Attack_Charge_Execute();
	if (m_pModelCom->Get_AnimationProgress() > 0.5f && m_pModelCom->Get_AnimationProgress() < 0.55f)
		Play_SFX_Attack_Charge_Done();

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Charge_Attack_Into_Run(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.2f)
		Move(dTimeDelta * 0.5f, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.3f && m_pModelCom->Get_AnimationProgress() < 0.5f)
		CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_GroundSlam", m_pTransformCom->Get_WorldMatrix());

	if (m_pModelCom->Get_AnimationProgress() > 0.10f && m_pModelCom->Get_AnimationProgress() < 0.15f)
		Play_Voice_Charge_Attack();
	if (m_pModelCom->Get_AnimationProgress() > 0.15f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Play_SFX_Attack_Charge_Start();
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.25f)
		Play_SFX_Attack_Charge_Execute();
	if (m_pModelCom->Get_AnimationProgress() > 0.5f && m_pModelCom->Get_AnimationProgress() < 0.55f)
		Play_SFX_Attack_Charge_Done();

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Charge_Attack_Into_Water_Idle(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.2f)
		Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.10f && m_pModelCom->Get_AnimationProgress() < 0.15f)
		Play_Voice_Charge_Attack();
	if (m_pModelCom->Get_AnimationProgress() > 0.15f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Play_SFX_Attack_Charge_Start();
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.25f)
		Play_SFX_Attack_Charge_Execute();
	if (m_pModelCom->Get_AnimationProgress() > 0.5f && m_pModelCom->Get_AnimationProgress() < 0.55f)
		Play_SFX_Attack_Charge_Done();

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Charge_Attack_Into_Water_Run(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.10f && m_pModelCom->Get_AnimationProgress() < 0.15f)
		Play_Voice_Charge_Attack();
	if (m_pModelCom->Get_AnimationProgress() > 0.15f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Play_SFX_Attack_Charge_Start();
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.25f)
		Play_SFX_Attack_Charge_Execute();
	if (m_pModelCom->Get_AnimationProgress() > 0.5f && m_pModelCom->Get_AnimationProgress() < 0.55f)
		Play_SFX_Attack_Charge_Done();

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Charge_Attack_Into_Surf(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.10f && m_pModelCom->Get_AnimationProgress() < 0.15f)
		Play_Voice_Charge_Attack();
	if (m_pModelCom->Get_AnimationProgress() > 0.15f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Play_SFX_Attack_Charge_Start();
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.25f)
		Play_SFX_Attack_Charge_Execute();
	if (m_pModelCom->Get_AnimationProgress() > 0.5f && m_pModelCom->Get_AnimationProgress() < 0.55f)
		Play_SFX_Attack_Charge_Done();

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Charge_Attack_Into_Surf_Fast(_double dTimeDelta)
{
	Move(dTimeDelta, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.10f && m_pModelCom->Get_AnimationProgress() < 0.15f)
		Play_Voice_Charge_Attack();
	if (m_pModelCom->Get_AnimationProgress() > 0.15f && m_pModelCom->Get_AnimationProgress() < 0.2f)
		Play_SFX_Attack_Charge_Start();
	if (m_pModelCom->Get_AnimationProgress() > 0.2f && m_pModelCom->Get_AnimationProgress() < 0.25f)
		Play_SFX_Attack_Charge_Execute();
	if (m_pModelCom->Get_AnimationProgress() > 0.5f && m_pModelCom->Get_AnimationProgress() < 0.55f)
		Play_SFX_Attack_Charge_Done();

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Ground_Slam_Intro(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.1f)
		Move(0.0, m_eDir);

	if (m_pModelCom->Get_AnimationProgress() > 0.96f)
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Ground_Slam_Fall(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);

	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Combat_Ground_Slam_Hit(_double dTimeDelta)
{
	if (m_pPlayer->m_bAttackHit == true)
	{
		Play_SFX_Hit();
		m_pPlayer->m_bAttackHit = false;
	}
}

void CSigrid_State::Tick_Damage_Death_Air_Fall_Ground(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Damage_Death_Air_Fall_Water(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Damage_Death_Air_Fall(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Damage_Death_Air(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Damage_Hit_Air(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Damage_Hit_Idle(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Fast(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Fast_45(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Fast_90(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Fast_Negative_45(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Fast_Negative_90(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Slow(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Slow_45(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Slow_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Slow_Negative_45(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Fire_Slow_Negative_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Fast(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Fast_45(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Fast_90(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Fast_Negative_45(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Fast_Negative_90(_double dTimeDelta)
{
	m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Slow(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
		
		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Slow_45(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Slow_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Slow_Negative_45(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Air_Fire_Slow_Negative_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() > 0.4f)
	{
		m_pTransformCom->Chase(m_pPlayer->m_vSnapGrapplePos, dTimeDelta, 0.f, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);

		if (m_pModelCom->Get_AnimationProgress() < 0.43f)
		{
			Play_SFX_HookShot_AimIn();
			Play_SFX_HookShot_In_Swish();
		}
	}

	_float		fDist = XMVectorGetX(XMVector3Length(m_pPlayer->m_vSnapGrapplePos - m_pTransformCom->Get_State(CTransform::STATE_TRANS)));
	if (fDist < 0.1f)
	{
		m_pPlayer->m_bReadyLaunch = true;
		Play_SFX_HookShot_Arrive();
	}
}

void CSigrid_State::Tick_Grapple_Ground_Aim(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Ground_Aim_Down(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Ground_Aim_Up(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Air_Aim(_double dTimeDelta)
{
	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
	Move(dTimeDelta, m_eDir);
}

void CSigrid_State::Tick_Grapple_Air_Aim_Down(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Air_Aim_Up(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Hang_Intro(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Hang(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Launch_Flip(_double dTimeDelta)
{
}

void CSigrid_State::Tick_Grapple_Launch(_double dTimeDelta)
{
	//if (m_pModelCom->Get_AnimationProgress() < 0.6f)
		m_pTransformCom->Throw(m_pPlayer->m_vSnapGrappleLook, 1.f, dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
	//else
	//	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
}

void CSigrid_State::Tick_Grapple_Launch_45(_double dTimeDelta)
{
	//if (m_pModelCom->Get_AnimationProgress() < 0.6f)
		m_pTransformCom->Throw(m_pPlayer->m_vSnapGrappleLook, 1.f, dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
	//else
	//	m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
}

void CSigrid_State::Tick_Grapple_Launch_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.6f)
		m_pTransformCom->Throw(m_pPlayer->m_vSnapGrappleLook, 1.f, dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
	else
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
}

void CSigrid_State::Tick_Grapple_Launch_Negative_45(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.6f)
		m_pTransformCom->Throw(m_pPlayer->m_vSnapGrappleLook, 1.f, dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
	else
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
}

void CSigrid_State::Tick_Grapple_Launch_Negative_90(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimationProgress() < 0.6f)
		m_pTransformCom->Throw(m_pPlayer->m_vSnapGrappleLook, 1.f, dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
	else
		m_pTransformCom->Jump(dTimeDelta, m_pPlayer->m_fGravity, m_pPlayer->m_fCurJumpSpeed);
}

void CSigrid_State::End_Ground_Idle(_double dTimeDelta)
{
}

void CSigrid_State::End_Dash_Into_Idle(_double dTimeDelta)
{
	m_dEffectCloneTimer = 0.0;
}

void CSigrid_State::End_Ground_Run(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Ground_Run_Left(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Ground_Run_Right(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Dash_Into_Run(_double dTimeDelta)
{
	m_dEffectCloneTimer = 0.0;
}

void CSigrid_State::End_Ground_Boost(_double dTimeDelta)
{
	CGameInstance::GetInstance()->Stop_Sound(9);
}

void CSigrid_State::End_Ground_Boost_Left(_double dTimeDelta)
{
	CGameInstance::GetInstance()->Stop_Sound(9);
}

void CSigrid_State::End_Ground_Boost_Right(_double dTimeDelta)
{
	CGameInstance::GetInstance()->Stop_Sound(9);
}

void CSigrid_State::End_Ground_Boost_Stop(_double dTimeDelta)
{
}

void CSigrid_State::End_Water_Idle(_double dTimeDelta)
{
}

void CSigrid_State::End_Water_Run(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationSpeed(25.0);
}

void CSigrid_State::End_Water_Run_Left(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationSpeed(25.0);
}

void CSigrid_State::End_Water_Run_Right(_double dTimeDelta)
{
	m_pModelCom->Set_CurAnimationSpeed(25.0);
}

void CSigrid_State::End_Water_Braking(_double dTimeDelta)
{
}

void CSigrid_State::End_Water_Diving_Exit(_double dTimeDelta)
{
}

void CSigrid_State::End_Water_Diving(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Intro(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Left(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Right(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Fast_Intro(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Fast(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Fast_Left(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Fast_Right(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Boost(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Boost_Left(_double dTimeDelta)
{
}

void CSigrid_State::End_Surf_Boost_Right(_double dTimeDelta)
{
}

void CSigrid_State::End_Snap_Turn_Ground_Idle(_double dTimeDelta)
{
}

void CSigrid_State::End_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta)
{
}

void CSigrid_State::End_Snap_Turn_Ground_Run(_double dTimeDelta)
{
}

void CSigrid_State::End_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta)
{
}

void CSigrid_State::End_Snap_Turn_Ground_Boost(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Double(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charging(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charging_Left(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charging_Right(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charged1_60(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charged1_90(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charged2_60(_double dTimeDelta)
{
}

void CSigrid_State::End_Jump_Charged2_90(_double dTimeDelta)
{
}

void CSigrid_State::End_Dash_Into_Air(_double dTimeDelta)
{
	m_dEffectCloneTimer = 0.0;
}

void CSigrid_State::End_Dash_Into_Water_Idle(_double dTimeDelta)
{
	m_dEffectCloneTimer = 0.0;
}

void CSigrid_State::End_Dash_Into_Surf(_double dTimeDelta)
{
	m_dEffectCloneTimer = 0.0;
}

void CSigrid_State::End_Dash_Into_Surf_Fast(_double dTimeDelta)
{
	m_dEffectCloneTimer = 0.0;
}

void CSigrid_State::End_Air(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Ascend_60(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Ascend_90(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Descend_60(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Descend_90(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Descend_Fast_60(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Descend_Fast_90(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Gliding(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 20.f;
}

void CSigrid_State::End_Air_Gliding_Left(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 20.f;
}

void CSigrid_State::End_Air_Gliding_Right(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 20.f;
}

void CSigrid_State::End_Air_Gliding_Left_Turn(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 20.f;
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Air_Gliding_Right_Turn(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 20.f;
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Landing_Ground_Into_Idle(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Landing_Ground_Into_Run(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Landing_Slope(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Landing_Surf_Fast(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Landing_Surf(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Landing_Water(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);
}

void CSigrid_State::End_Combat_Combo1_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo1_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo1_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo1_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo2_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo2_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo2_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo2_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo3_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo3_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo3_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo3_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo4_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo4_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo4_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Combo4_Air(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Charge_Attack_Into_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Charge_Attack_Into_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Charge_Attack_Into_Water_Idle(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Charge_Attack_Into_Water_Run(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Charge_Attack_Into_Surf(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Charge_Attack_Into_Surf_Fast(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Ground_Slam_Intro(_double dTimeDelta)
{
	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Ground_Slam_Fall(_double dTimeDelta)
{
	m_pPlayer->m_fGravity /= 10.f;

	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Combat_Ground_Slam_Hit(_double dTimeDelta)
{
	m_pModelCom->Set_LerpTime(0.2f);

	m_pPlayer->m_bAttack = false;
	m_pPlayer->m_bSpecialAttack = false;
}

void CSigrid_State::End_Damage_Death_Air_Fall_Ground(_double dTimeDelta)
{
}

void CSigrid_State::End_Damage_Death_Air_Fall_Water(_double dTimeDelta)
{
}

void CSigrid_State::End_Damage_Death_Air_Fall(_double dTimeDelta)
{
}

void CSigrid_State::End_Damage_Death_Air(_double dTimeDelta)
{
}

void CSigrid_State::End_Damage_Hit_Air(_double dTimeDelta)
{
}

void CSigrid_State::End_Damage_Hit_Idle(_double dTimeDelta)
{
}

void CSigrid_State::End_Grapple_Ground_Fire(_double dTimeDelta)
{
	m_pPlayer->m_bSnapGrappleSlow = false;
	m_pPlayer->m_bSnapGrappleFast = false;

	if (m_pModelCom->Get_CurAnimationIndex() == SURF || m_pModelCom->Get_CurAnimationIndex() == SURF_LEFT || m_pModelCom->Get_CurAnimationIndex() == SURF_RIGHT ||
		m_pModelCom->Get_CurAnimationIndex() == SURF_FAST || m_pModelCom->Get_CurAnimationIndex() == SURF_FAST_LEFT || m_pModelCom->Get_CurAnimationIndex() == SURF_FAST_RIGHT ||
		m_pModelCom->Get_CurAnimationIndex() == SURF_BOOST|| m_pModelCom->Get_CurAnimationIndex() == SURF_BOOST_LEFT || m_pModelCom->Get_CurAnimationIndex() == SURF_BOOST_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(0);

	if (m_pModelCom->Get_CurAnimationIndex() == JUMP_CHARGING || m_pModelCom->Get_CurAnimationIndex() == JUMP_CHARGING_LEFT || m_pModelCom->Get_CurAnimationIndex() == JUMP_CHARGING_RIGHT)
		CGameInstance::GetInstance()->Stop_Sound(6);
}

void CSigrid_State::End_Grapple_Ground_Fire_Fast(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Fast_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Fast_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Fast_Negative_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Fast_Negative_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Slow(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Slow_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Slow_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Slow_Negative_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Fire_Slow_Negative_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire(_double dTimeDelta)
{
	m_pPlayer->m_bSnapGrappleSlow = false;
	m_pPlayer->m_bSnapGrappleFast = false;

	if (m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING || m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING_RIGHT ||
		m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING_LEFT || m_pModelCom->Get_CurAnimationIndex() == AIR_GLIDING_RIGHT_TURN)
		CGameInstance::GetInstance()->Stop_Sound(2);
}

void CSigrid_State::End_Grapple_Air_Fire_Fast(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Fast_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Fast_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Fast_Negative_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Fast_Negative_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Slow(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Slow_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Slow_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Slow_Negative_45(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Air_Fire_Slow_Negative_90(_double dTimeDelta)
{
	if (m_pStateMachine->Get_NextState() == L"AIR")
	{
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		m_pPlayer->m_dPauseTime = 0.0;
	}

	m_pPlayer->m_bReadyLaunch = false;
}

void CSigrid_State::End_Grapple_Ground_Aim(_double dTimeDelta)
{
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pPlayer->m_dTimeScale = 1.0;
}

void CSigrid_State::End_Grapple_Ground_Aim_Down(_double dTimeDelta)
{
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pPlayer->m_dTimeScale = 1.0;
	m_pPlayer->m_pCamera->Set_TimeScale(1.0);
}

void CSigrid_State::End_Grapple_Ground_Aim_Up(_double dTimeDelta)
{
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pPlayer->m_dTimeScale = 1.0;
	m_pPlayer->m_pCamera->Set_TimeScale(1.0);
}

void CSigrid_State::End_Grapple_Air_Aim(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 40.f;
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pPlayer->m_dTimeScale = 1.0;
	m_pPlayer->m_pCamera->Set_TimeScale(1.0);
}

void CSigrid_State::End_Grapple_Air_Aim_Down(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 40.f;
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pPlayer->m_dTimeScale = 1.0;
	m_pPlayer->m_pCamera->Set_TimeScale(1.0);
}

void CSigrid_State::End_Grapple_Air_Aim_Up(_double dTimeDelta)
{
	m_pPlayer->m_fGravity *= 40.f;
	m_pGameInstance->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pPlayer->m_dTimeScale = 1.0;
	m_pPlayer->m_pCamera->Set_TimeScale(1.0);
}

void CSigrid_State::End_Grapple_Hang_Intro(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleHang = false;
}

void CSigrid_State::End_Grapple_Hang(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleHang = false;
}

void CSigrid_State::End_Grapple_Launch_Flip(_double dTimeDelta)
{
}

void CSigrid_State::End_Grapple_Launch(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleLauncher = false;
}

void CSigrid_State::End_Grapple_Launch_45(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleLauncher = false;
}

void CSigrid_State::End_Grapple_Launch_90(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleLauncher = false;
}

void CSigrid_State::End_Grapple_Launch_Negative_45(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleLauncher = false;
}

void CSigrid_State::End_Grapple_Launch_Negative_90(_double dTimeDelta)
{
	m_pPlayer->m_bReadyLaunch = false;
	m_pPlayer->m_bGrappleLauncher = false;
}

_bool CSigrid_State::KeyInput_None()
{
	_bool		bDetectInput = true;

	if (m_eDir != CTransform::DIR_END)
		bDetectInput = false;

	return bDetectInput;
}

_bool CSigrid_State::KeyInput_Direction()
{
	_bool		bDetectInput = false;

	if (m_pGameInstance->Key_Pressing(DIK_W))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_A))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_S))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_D))
		bDetectInput = true;

	if (Turn_Back())
		bDetectInput = false;

	return bDetectInput;
}

_bool CSigrid_State::KeyInput_Direction_Progress()
{
	_bool		bDetectInput = false;

	if (m_pModelCom->Get_AnimationProgress() < 0.55f)
		return bDetectInput;

	if (m_pGameInstance->Key_Pressing(DIK_W))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_A))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_S))
		bDetectInput = true;
	if (m_pGameInstance->Key_Pressing(DIK_D))
		bDetectInput = true;


	return bDetectInput;
}

_bool CSigrid_State::KeyInput_W()
{
	if (m_pGameInstance->Key_Pressing(DIK_W) && !KeyInput_WA() && !KeyInput_WD())
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_A()
{
	if (m_pGameInstance->Key_Pressing(DIK_A) && !KeyInput_WA() && !KeyInput_SA())
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_S()
{
	if (m_pGameInstance->Key_Pressing(DIK_S) && !KeyInput_SA() && !KeyInput_SD())
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_D()
{
	if (m_pGameInstance->Key_Pressing(DIK_D) && !KeyInput_WD() && !KeyInput_SD())
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_WA()
{
	if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_A))
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_WD()
{
	if (m_pGameInstance->Key_Pressing(DIK_W) && m_pGameInstance->Key_Pressing(DIK_D))
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_SA()
{
	if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_A))
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_SD()
{
	if (m_pGameInstance->Key_Pressing(DIK_S) && m_pGameInstance->Key_Pressing(DIK_D))
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_WtoS()
{
	if (m_eDir == CTransform::DIR_W || m_eDir == CTransform::DIR_WA || m_eDir == CTransform::DIR_WD)
	{
		if (m_pGameInstance->Key_Pressing(DIK_S))
			return true;
	}

	return false;
}

_bool CSigrid_State::KeyInput_StoW()
{
	if (m_eDir == CTransform::DIR_S || m_eDir == CTransform::DIR_SA || m_eDir == CTransform::DIR_SD)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
			return true;
	}

	return false;
}

_bool CSigrid_State::KeyInput_AtoD()
{
	if (m_eDir == CTransform::DIR_A || m_eDir == CTransform::DIR_WA || m_eDir == CTransform::DIR_SA)
	{
		if (m_pGameInstance->Key_Pressing(DIK_D))
			return true;
	}

	return false;
}

_bool CSigrid_State::KeyInput_DtoA()
{
	if (m_eDir == CTransform::DIR_D || m_eDir == CTransform::DIR_WD || m_eDir == CTransform::DIR_SD)
	{
		if (m_pGameInstance->Key_Pressing(DIK_A))
			return true;
	}

	return false;
}

_bool CSigrid_State::KeyInput_Space()
{
	if (m_pGameInstance->Key_Pressing(DIK_SPACE))
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_Space_Progress()
{
	if (m_pGameInstance->Key_Pressing(DIK_SPACE) && m_pModelCom->Get_AnimationProgress() > 0.65f)
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_Shift()
{
	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		return true;

	return false;
}

_bool CSigrid_State::KeyInput_LCTRL()
{
	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		return true;

	return false;
}

_bool CSigrid_State::KeyDown_F()
{
	if (m_pGameInstance->Key_Down(DIK_F) && m_pPlayer->m_bDash == false)
		return true;

	return false;
}

_bool CSigrid_State::KeyDown_Space()
{
	if (m_pGameInstance->Key_Down(DIK_SPACE))
		return true;

	return false;
}

_bool CSigrid_State::KeyDown_Space_Progress()
{
	if (m_pGameInstance->Key_Down(DIK_SPACE) && m_pModelCom->Get_AnimationProgress() > 0.65f)
		return true;

	return false;
}

_bool CSigrid_State::KeyDown_LCTRL()
{
	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
		return true;

	return false;
}

_bool CSigrid_State::KeyDown_Escape()
{
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
		return true;

	return false;
}

_bool CSigrid_State::KeyDoubleDown_Space()
{
	if (m_pGameInstance->Key_DoubleDown(DIK_SPACE) && m_pPlayer->m_bDoubleJump == false)
		return true;

	return false;
}

_bool CSigrid_State::KeyDoubleDown_Space_Progress()
{
	if (m_pGameInstance->Key_DoubleDown(DIK_SPACE) && m_pPlayer->m_bDoubleJump == false && m_pModelCom->Get_AnimationProgress() > 0.25f)
		return true;

	return false;
}

_bool CSigrid_State::KeyUp_A()
{
	if (m_pGameInstance->Key_Up(DIK_A))
		return true;

	return false;
}

_bool CSigrid_State::KeyUp_D()
{
	if (m_pGameInstance->Key_Up(DIK_D))
		return true;

	return false;
}

_bool CSigrid_State::KeyUp_Space()
{
	if (m_pGameInstance->Key_Up(DIK_SPACE))
		return true;

	return false;
}

_bool CSigrid_State::KeyUp_Shift()
{
	if (m_pGameInstance->Key_Up(DIK_LSHIFT))
	{
		m_pPlayer->m_bBoost = false;
		return true;
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space()
{
	if (m_pGameInstance->Key_Charge(DIK_SPACE, 0.3))
		return true;

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Progress()
{
	if (m_pModelCom->Get_AnimationProgress() > 0.45f && m_pGameInstance->Key_Charge(DIK_SPACE, 0.3))
		return true;

	return false;
}

_bool CSigrid_State::MouseDown_LB()
{
	if (m_pGameInstance->Mouse_Down(DIM_LB))
		return true;

	return false;
}

_bool CSigrid_State::MousePressing_RB()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RB))
		return true;

	return false;
}

_bool CSigrid_State::MouseDown_RB()
{
	if (m_pGameInstance->Mouse_Down(DIM_RB))
		return true;

	return false;
}

_bool CSigrid_State::MouseUp_RB()
{
	if (m_pGameInstance->Mouse_Up(DIM_RB))
		return true;

	return false;
}

_bool CSigrid_State::KeyDown_Space_While_Dash_Into_Air()
{
	if (m_pPlayer->m_bDoubleJump == false && m_pGameInstance->Key_Down(DIK_SPACE))
		return true;

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_00()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 0.0 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 0.5 && KeyInput_None())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = m_pPlayer->m_fInitJumpSpeed;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_00_With_Dir()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 0.0 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 0.5 && KeyInput_Direction())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = m_pPlayer->m_fInitJumpSpeed;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_05()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 0.5 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 1.0 && KeyInput_None())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 1.7f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_05_With_Dir()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 0.5 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 1.0 && KeyInput_Direction())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 1.7f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_10()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 1.0 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 1.5 && KeyInput_None())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 1.9f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_10_With_Dir()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 1.0 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 1.5 && KeyInput_Direction())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 1.9f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_15()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 1.5 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 2.0 && KeyInput_None())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 2.1f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_15_With_Dir()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 1.5 && CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) < 2.0 && KeyInput_Direction())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 2.1f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_20()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 2.0 && KeyInput_None())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 2.5f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::KeyCharge_Space_Jump_Charge_20_With_Dir()
{
	if (CGameInstance::GetInstance()->Get_ChargeTime(DIK_SPACE) >= 2.0 && KeyInput_Direction())
	{
		if (CGameInstance::GetInstance()->Key_Up(DIK_SPACE))
		{
			m_pPlayer->m_fCurJumpSpeed = 2.5f;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::Air0()
{
	if (m_pPlayer->m_fCurJumpSpeed > -0.8f && m_pPlayer->m_fCurJumpSpeed <= 0.2f)
		return true;

	return false;
}

_bool CSigrid_State::AirAscend60()
{
	if (m_pPlayer->m_fCurJumpSpeed > 0.2f && KeyInput_Direction())
		return true;

	return false;
}

_bool CSigrid_State::AirAscend90()
{
	if (m_pPlayer->m_fCurJumpSpeed > 0.2f && KeyInput_None())
		return true;

	return false;
}

_bool CSigrid_State::AirDescend_60()
{
	if (m_pPlayer->m_fCurJumpSpeed > -2.5f && m_pPlayer->m_fCurJumpSpeed <= -0.8f && KeyInput_Direction())
		return true;

	return false;
}

_bool CSigrid_State::AirDescend_90()
{
	if (m_pPlayer->m_fCurJumpSpeed > -2.5f && m_pPlayer->m_fCurJumpSpeed <= -0.8f && KeyInput_None())
		return true;

	return false;
}

_bool CSigrid_State::AirDescendFast()
{
	if (m_pPlayer->m_fCurJumpSpeed <= -2.5f)
		return true;

	return false;
}

_bool CSigrid_State::Damaged()
{
	return m_pPlayer->m_bHit;
}

_bool CSigrid_State::SnapGrapple()
{
	return m_pPlayer->m_bSnapGrappleFast || m_pPlayer->m_bSnapGrappleSlow;
}

_bool CSigrid_State::SnapGrappleFast()
{
	if (m_pPlayer->m_bSnapGrappleFast)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == XMConvertToRadians(0.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleFast45()
{
	if (m_pPlayer->m_bSnapGrappleFast)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == XMConvertToRadians(45.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleFast90()
{
	if (m_pPlayer->m_bSnapGrappleFast)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == XMConvertToRadians(90.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleFastNeg45()
{
	if (m_pPlayer->m_bSnapGrappleFast)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == -XMConvertToRadians(45.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleFastNeg90()
{
	if (m_pPlayer->m_bSnapGrappleFast)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == -XMConvertToRadians(90.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleSlow()
{
	if (m_pPlayer->m_bSnapGrappleSlow)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == XMConvertToRadians(0.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleSlow45()
{
	if (m_pPlayer->m_bSnapGrappleSlow)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == XMConvertToRadians(45.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleSlow90()
{
	if (m_pPlayer->m_bSnapGrappleSlow)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == XMConvertToRadians(90.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleSlowNeg45()
{
	if (m_pPlayer->m_bSnapGrappleSlow)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == -XMConvertToRadians(45.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::SnapGrappleSlowNeg90()
{
	if (m_pPlayer->m_bSnapGrappleSlow)
	{
		if (m_pPlayer->m_fSnapGrappleAngle == -XMConvertToRadians(90.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::GrappleLaunchAngle0()
{
	//if (m_pPlayer->m_bReadyLaunch)
	{
		if (m_pPlayer->m_fLauncherAngle == XMConvertToRadians(0.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::GrappleLaunchAngle45()
{
	//if (m_pPlayer->m_bReadyLaunch)
	{
		if (m_pPlayer->m_fLauncherAngle == XMConvertToRadians(45.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::GrappleLaunchAngle90()
{
	//if (m_pPlayer->m_bReadyLaunch)
	{
		if (m_pPlayer->m_fLauncherAngle == XMConvertToRadians(90.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::GrappleLaunchAngleNeg45()
{
	//if (m_pPlayer->m_bReadyLaunch)
	{
		if (m_pPlayer->m_fLauncherAngle == -XMConvertToRadians(45.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::GrappleLaunchAngleNeg90()
{
	//if (m_pPlayer->m_bReadyLaunch)
	{
		if (m_pPlayer->m_fLauncherAngle == -XMConvertToRadians(90.f))
			return true;
	}

	return false;
}

_bool CSigrid_State::Grapple_Done()
{
	return m_pPlayer->m_bReadyLaunch && !m_pPlayer->m_bGrappleLauncher && !m_pPlayer->m_bGrappleHang;
}

_bool CSigrid_State::ReadyLaunch()
{
	return m_pPlayer->m_bReadyLaunch && m_pPlayer->m_bGrappleLauncher && !m_pPlayer->m_bGrappleHang;
}

_bool CSigrid_State::ReadyHang()
{
	return m_pPlayer->m_bReadyLaunch && !m_pPlayer->m_bGrappleLauncher && m_pPlayer->m_bGrappleHang;
}

_bool CSigrid_State::Turn_Back()
{
	_bool		bTurnBack = false;
	_matrix	matCamWorld = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix());

	_vector	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook = XMVectorSet(XMVectorGetX(vPlayerLook), 0.f, XMVectorGetZ(vPlayerLook), 0.f);

	_float4	vDir;

	switch (m_eDir)
	{
	case CTransform::DIR_W:
		vDir = XMVector3Normalize(matCamWorld.r[2]);
		break;

	case CTransform::DIR_A:
		vDir = XMVector3Normalize(matCamWorld.r[0]) * -1.f;
		break;

	case CTransform::DIR_S:
		vDir = XMVector3Normalize(matCamWorld.r[2]) * -1.f;
		break;

	case CTransform::DIR_D:
		vDir = XMVector3Normalize(matCamWorld.r[0]);
		break;

	case CTransform::DIR_WA:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_WD:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0]);
		break;

	case CTransform::DIR_SA:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_SD:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0]);
		break;
	}

	vDir.y = 0.f;

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vDir))));

	if (fAngle > 2.9f)
	{
		bTurnBack = true;
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
	}

	return bTurnBack;
}

_bool CSigrid_State::Turn_Back_Left()
{
	if (m_eDir == CTransform::DIR_END)
		return false;

	_bool		bTurnBack = false;
	_matrix	matCamWorld = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix());

	_vector	vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vPlayerRight = XMVectorSet(XMVectorGetX(vPlayerRight), 0.f, XMVectorGetZ(vPlayerRight), 0.f);

	_vector	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook = XMVectorSet(XMVectorGetX(vPlayerLook), 0.f, XMVectorGetZ(vPlayerLook), 0.f);

	_float4	vDir;

	switch (m_eDir)
	{
	case CTransform::DIR_W:
		vDir = XMVector3Normalize(matCamWorld.r[2]);
		break;

	case CTransform::DIR_A:
		vDir = XMVector3Normalize(matCamWorld.r[0]) * -1.f;
		break;

	case CTransform::DIR_S:
		vDir = XMVector3Normalize(matCamWorld.r[2]) * -1.f;
		break;

	case CTransform::DIR_D:
		vDir = XMVector3Normalize(matCamWorld.r[0]);
		break;

	case CTransform::DIR_WA:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_WD:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0]);
		break;

	case CTransform::DIR_SA:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_SD:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0]);
		break;
	}

	vDir.y = 0.f;

	_float		fCheckAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerRight), XMVector3Normalize(vDir)));
	if (fCheckAngle > 0.f)
		return false;

	_float		fAngle = XM_2PI - acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vDir))));

	if (fAngle > XM_PI && fAngle < XMConvertToRadians(260.f))
	{
		bTurnBack = true;
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
	}

	return bTurnBack;
}

_bool CSigrid_State::Turn_Back_Right()
{
	if (m_eDir == CTransform::DIR_END)
		return false;

	_bool		bTurnBack = false;
	_matrix	matCamWorld = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix());

	_vector	vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	vPlayerRight = XMVectorSet(XMVectorGetX(vPlayerRight), 0.f, XMVectorGetZ(vPlayerRight), 0.f);

	_vector	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook = XMVectorSet(XMVectorGetX(vPlayerLook), 0.f, XMVectorGetZ(vPlayerLook), 0.f);

	_float4	vDir;

	switch (m_eDir)
	{
	case CTransform::DIR_W:
		vDir = XMVector3Normalize(matCamWorld.r[2]);
		break;

	case CTransform::DIR_A:
		vDir = XMVector3Normalize(matCamWorld.r[0]) * -1.f;
		break;

	case CTransform::DIR_S:
		vDir = XMVector3Normalize(matCamWorld.r[2]) * -1.f;
		break;

	case CTransform::DIR_D:
		vDir = XMVector3Normalize(matCamWorld.r[0]);
		break;

	case CTransform::DIR_WA:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_WD:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0]);
		break;

	case CTransform::DIR_SA:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_SD:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0]);
		break;
	}

	vDir.y = 0.f;

	_float		fCheckAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerRight), XMVector3Normalize(vDir)));
	if (fCheckAngle < 0.f)
		return false;

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vDir))));

	if (fAngle > XMConvertToRadians(80.f) && fAngle <= XM_PI)
	{
		bTurnBack = true;
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
	}

	return bTurnBack;
}

_bool CSigrid_State::Check_SurfTime()
{
	if (m_pPlayer->m_dSurfTime > 3.0)
		return true;

	return false;
}

_bool CSigrid_State::Camera_Angle_Coincide()
{
	_float4	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.y = 0.f;
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	_float4	vCameraRight = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix()).r[0];
	vCameraRight.y = 0.f;
	vCameraRight = XMVector3Normalize(vCameraRight);

	_float4	vCameraLook = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix()).r[2];
	vCameraLook.y = 0.f;
	vCameraLook = XMVector3Normalize(vCameraLook);

	_float4	vCheckDir;
	_float4	vCameraDir;

	switch (m_eDir)
	{
	case CTransform::DIR_W:
		vCameraDir = vCameraLook;
		break;

	case CTransform::DIR_A:
		vCameraDir = vCameraRight * -1.f;
		break;

	case CTransform::DIR_S:
		vCameraDir = vCameraLook * -1.f;
		break;

	case CTransform::DIR_D:
		vCameraDir = vCameraRight;
		break;

	case CTransform::DIR_WA:
		vCameraDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook);
		break;

	case CTransform::DIR_WD:
		vCameraDir = XMVector3Normalize(vCameraRight + vCameraLook);
		break;

	case CTransform::DIR_SA:
		vCameraDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook * -1.f);
		break;

	case CTransform::DIR_SD:
		vCameraDir = XMVector3Normalize(vCameraRight + vCameraLook * -1.f);
		break;
	}

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(vPlayerLook, vCameraDir)));

	if (fAngle < XMConvertToRadians(10.f))
		return true;

	fAngle += XM_PI;

	if (fAngle > XMConvertToRadians(350.f))
		return true;

	return false;
}

_bool CSigrid_State::Camera_Angle_Turn_Left()
{
	if (m_eDir == CTransform::DIR_END)
		return false;

	_float4	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.y = 0.f;
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	_float4	vCameraRight = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix()).r[0];
	vCameraRight.y = 0.f;
	vCameraRight = XMVector3Normalize(vCameraRight);

	_float4	vCameraLook = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix()).r[2];
	vCameraLook.y = 0.f;
	vCameraLook = XMVector3Normalize(vCameraLook);

	_float4	vCheckDir;
	_float4	vCameraDir;

	switch (m_eDir)
	{
	case CTransform::DIR_W:
		vCheckDir = vCameraRight;
		vCameraDir = vCameraLook;
		break;

	case CTransform::DIR_A:
		vCheckDir = vCameraLook;
		vCameraDir = vCameraRight * -1.f;
		break;

	case CTransform::DIR_S:
		vCheckDir = vCameraRight * -1.f;
		vCameraDir = vCameraLook * -1.f;
		break;

	case CTransform::DIR_D:
		vCheckDir = vCameraLook * -1.f;
		vCameraDir = vCameraRight;
		break;

	case CTransform::DIR_WA:
		vCheckDir = XMVector3Normalize(vCameraRight + vCameraLook);
		vCameraDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook);
		break;

	case CTransform::DIR_WD:
		vCheckDir = XMVector3Normalize(vCameraRight + vCameraLook * -1.f);
		vCameraDir = XMVector3Normalize(vCameraRight + vCameraLook);
		break;

	case CTransform::DIR_SA:
		vCheckDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook);
		vCameraDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook * -1.f);
		break;

	case CTransform::DIR_SD:
		vCheckDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook * -1.f);
		vCameraDir = XMVector3Normalize(vCameraRight + vCameraLook * -1.f);
		break;
	}

	_float		fCheckDir = XMVectorGetX(XMVector3Dot(vPlayerLook, vCheckDir));

	if (fCheckDir < 0.f)
		return false;

	_float		fAngle = XM_2PI - acosf(XMVectorGetX(XMVector3Dot(vPlayerLook, vCameraDir)));

	if (fAngle > XMConvertToRadians(190.f) && fAngle < XMConvertToRadians(350.f))
		return true;

	return false;
}

_bool CSigrid_State::Camera_Angle_Turn_Right()
{
	if (m_eDir == CTransform::DIR_END)
		return false;

	_float4	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.y = 0.f;
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	_float4	vCameraRight = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix()).r[0];
	vCameraRight.y = 0.f;
	vCameraRight = XMVector3Normalize(vCameraRight);

	_float4	vCameraLook = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix()).r[2];
	vCameraLook.y = 0.f;
	vCameraLook = XMVector3Normalize(vCameraLook);

	_float4	vCheckDir;
	_float4	vCameraDir;

	switch (m_eDir)
	{
	case CTransform::DIR_W:
		vCheckDir = vCameraRight;
		vCameraDir = vCameraLook;
		break;

	case CTransform::DIR_A:
		vCheckDir = vCameraLook;
		vCameraDir = vCameraRight * -1.f;
		break;

	case CTransform::DIR_S:
		vCheckDir = vCameraRight * -1.f;
		vCameraDir = vCameraLook * -1.f;
		break;

	case CTransform::DIR_D:
		vCheckDir = vCameraLook* -1.f;
		vCameraDir = vCameraRight;
		break;

	case CTransform::DIR_WA:
		vCheckDir = XMVector3Normalize(vCameraRight + vCameraLook);
		vCameraDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook);
		break;

	case CTransform::DIR_WD:
		vCheckDir = XMVector3Normalize(vCameraRight + vCameraLook * -1.f);
		vCameraDir = XMVector3Normalize(vCameraRight + vCameraLook);
		break;

	case CTransform::DIR_SA:
		vCheckDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook);
		vCameraDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook * -1.f);
		break;

	case CTransform::DIR_SD:
		vCheckDir = XMVector3Normalize(vCameraRight * -1.f + vCameraLook * -1.f);
		vCameraDir = XMVector3Normalize(vCameraRight + vCameraLook * -1.f);
		break;
	}

	_float		fCheckDir = XMVectorGetX(XMVector3Dot(vPlayerLook, vCheckDir));

	if (fCheckDir > 0.f)
		return false;

	_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(vPlayerLook, vCameraDir)));

	if (fAngle > XMConvertToRadians(10.f) && fAngle < XMConvertToRadians(170.f))
		return true;

	return false;
}

_bool CSigrid_State::Animation_Finish()
{
	return m_pModelCom->Get_AnimationFinish();
}

_bool CSigrid_State::Animation_Progress_30()
{
	return m_pModelCom->Get_AnimationProgress() > 0.3f;
}

_bool CSigrid_State::Animation_Progress_30_KeyInput_Direction()
{
	if (m_pModelCom->Get_AnimationProgress() > 0.3f && KeyInput_Direction())
		return true;

	return false;
}

_bool CSigrid_State::Animation_Progress_30_KeyInput_None()
{
	if (m_pModelCom->Get_AnimationProgress() > 0.3f && KeyInput_None())
		return true;

	return false;
}

_bool CSigrid_State::Animation_Progress_35()
{
	return m_pModelCom->Get_AnimationProgress() > 0.35f;
}

_bool CSigrid_State::Animation_Progress_40()
{
	return m_pModelCom->Get_AnimationProgress() > 0.4f;
}

_bool CSigrid_State::Animation_Progress_40_KeyInput_Direction()
{
	return m_pModelCom->Get_AnimationProgress() > 0.4f && KeyInput_Direction();
}

_bool CSigrid_State::Animation_Progress_40_KeyInput_None()
{
	return m_pModelCom->Get_AnimationProgress() > 0.4f && KeyInput_None();
}

_bool CSigrid_State::Animation_Progress_50()
{
	return m_pModelCom->Get_AnimationProgress() > 0.5f;
}

_bool CSigrid_State::Animation_Progress_50_KeyInput_Direction()
{
	return m_pModelCom->Get_AnimationProgress() > 0.5f && KeyInput_Direction();
}

_bool CSigrid_State::Animation_Progress_50_KeyInput_None()
{
	return m_pModelCom->Get_AnimationProgress() > 0.5f && KeyInput_None();
}

_bool CSigrid_State::Animation_Progress_60()
{
	return m_pModelCom->Get_AnimationProgress() > 0.6f;
}

_bool CSigrid_State::Animation_Progress_60_KeyInput_Direction()
{
	return m_pModelCom->Get_AnimationProgress() > 0.6f && KeyInput_Direction();
}

_bool CSigrid_State::Animation_Progress_60_KeyInput_None()
{
	return m_pModelCom->Get_AnimationProgress() > 0.6f && KeyInput_None();
}

_bool CSigrid_State::Animation_Progress_70()
{
	return m_pModelCom->Get_AnimationProgress() > 0.7f;
}

_bool CSigrid_State::Animation_Progress_80()
{
	return m_pModelCom->Get_AnimationProgress() > 0.8f;
}

_bool CSigrid_State::Animation_Progress_90()
{
	return m_pModelCom->Get_AnimationProgress() > 0.9f;
}

_bool CSigrid_State::Collision_Ground()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (vPlayerPos.y < m_pPlayer->m_fGroundHeight)
	{
		m_pPlayer->m_bBoost = false;
		m_pPlayer->m_bJump = false;
		m_pPlayer->m_bDoubleJump = false;
		m_pPlayer->m_bDash = false;
		return true;
	}

	return false;
}

_bool CSigrid_State::Collision_Ground_KeyInput_None()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_GROUND ||
		m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_ROOF)
	{
		if (vPlayerPos.y < m_pPlayer->m_fGroundHeight && KeyInput_None())
		{
			m_pPlayer->m_bBoost = false;
			m_pPlayer->m_bJump = false;
			m_pPlayer->m_bDoubleJump = false;
			m_pPlayer->m_bDash = false;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::Collision_Ground_KeyInput_Direction()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_GROUND ||
		m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_ROOF)
	{
		if (vPlayerPos.y < m_pPlayer->m_fGroundHeight && KeyInput_Direction())
		{
			m_pPlayer->m_bBoost = false;
			m_pPlayer->m_bJump = false;
			m_pPlayer->m_bDoubleJump = false;
			m_pPlayer->m_bDash = false;
			return true;
		}
	}

	return false;
}

_bool CSigrid_State::Collision_Ocean_KeyInput_None()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() != CCell::STATE_OCEAN)
		return false;

	if (vPlayerPos.y < m_pPlayer->m_fGroundHeight && KeyInput_None())
	{
		m_pPlayer->m_bBoost = false;
		m_pPlayer->m_bJump = false;
		m_pPlayer->m_bDoubleJump = false;
		m_pPlayer->m_bDash = false;
		return true;
	}

	return false;
}

_bool CSigrid_State::Collision_Ocean_KeyInput_Direction()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() != CCell::STATE_OCEAN)
		return false;

	if (vPlayerPos.y < m_pPlayer->m_fGroundHeight && KeyInput_Direction() && m_pPlayer->m_dSurfTime <= 3.0 && m_pPlayer->m_bBoost == false)
	{
		m_pPlayer->m_bJump = false;
		m_pPlayer->m_bDoubleJump = false;
		m_pPlayer->m_bDash = false;
		return true;
	}

	return false;
}

_bool CSigrid_State::Collision_Ocean_KeyInput_Direction_SurfTime()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() != CCell::STATE_OCEAN)
		return false;

	if (vPlayerPos.y < m_pPlayer->m_fGroundHeight && KeyInput_Direction() && m_pPlayer->m_dSurfTime > 3.0 && m_pPlayer->m_bBoost == false)
	{
		m_pPlayer->m_bJump = false;
		m_pPlayer->m_bDoubleJump = false;
		m_pPlayer->m_bDash = false;
		return true;
	}

	return false;
}

_bool CSigrid_State::Collision_Ocean_Boost()
{
	_float4		vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() != CCell::STATE_OCEAN)
		return false;

	if (vPlayerPos.y < m_pPlayer->m_fGroundHeight && KeyInput_Direction() && m_pPlayer->m_bBoost == true)
	{
		m_pPlayer->m_bJump = false;
		m_pPlayer->m_bDoubleJump = false;
		m_pPlayer->m_bDash = false;
		return true;
	}

	return false;
}

_bool CSigrid_State::IsOnGround()
{
	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_GROUND ||
		m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_ROOF)
		return true;

	return false;
}

_bool CSigrid_State::IsOnOcean()
{
	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_OCEAN)
		return true;

	return false;
}

_bool CSigrid_State::IsOnAir()
{
	if (m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_AIR)
	{
		m_pPlayer->m_bJump = true;
		m_pPlayer->m_bDoubleJump = false;
		m_pPlayer->m_bDash = false;
		m_pPlayer->m_fCurJumpSpeed = 0.f;
		return true;
	}

	return false;
}

void CSigrid_State::Play_SFX_Attack_Charge_Start()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Start-001.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Start-002.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Start-003.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Start-004.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Start-005.wav", g_fSFXVolume, false, false, 1);
}

void CSigrid_State::Play_SFX_Attack_Charge_Execute()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Execute-001.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Execute-002.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Execute-003.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Execute-004.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Execute-005.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Play_SFX_Attack_Charge_Done()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Done-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Done-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Done-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Done-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Charge_Done-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_Attack_GroundSlam_Intro()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Intro-001.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Intro-002.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Intro-003.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Intro-004.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Intro-005.wav", g_fSFXVolume, false, false, 1);
}

void CSigrid_State::Play_SFX_Attack_GroundSlam_Hit()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Hit-001.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Hit-002.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Hit-003.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Hit-004.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_GroundSlam_Hit-005.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Play_SFX_Attack_Combo_1_3()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_First-001.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_First-002.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_First-003.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_First-004.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_First-005.wav", g_fSFXVolume, false, true, 4);
}

void CSigrid_State::Play_SFX_Attack_Combo_2_4()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_Second-001.wav", g_fSFXVolume, false, true, 5);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_Second-002.wav", g_fSFXVolume, false, true, 5);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_Second-003.wav", g_fSFXVolume, false, true, 5);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_Second-004.wav", g_fSFXVolume, false, true, 5);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Attack_Swing_Second-005.wav", g_fSFXVolume, false, true, 5);
}

void CSigrid_State::Play_SFX_Damaged()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_Generic-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_Generic-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_Generic-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_Generic-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_Generic-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_Hit()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_WallHit-001.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_WallHit-002.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_WallHit-003.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_WallHit-004.wav", g_fSFXVolume, false, true, 4);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Damage_WallHit-005.wav", g_fSFXVolume, false, true, 4);
}

void CSigrid_State::Play_SFX_Movement_Glide_Fly_Start()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Start-001.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Start-002.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Start-003.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Start-004.wav", g_fSFXVolume, false, false, 1);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Glide_Fly_Start-005.wav", g_fSFXVolume, false, false, 1);
}

void CSigrid_State::Play_SFX_HookShot_AimIn()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_AimIn-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_AimIn-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_AimIn-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_AimIn-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_AimIn-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_HookShot_Arrive()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Arrive-001.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Arrive-002.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Arrive-003.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Arrive-004.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Arrive-005.wav", g_fSFXVolume, false, false, 4);
}

void CSigrid_State::Play_SFX_HookShot_In_Swish()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_In_Swish-001.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_In_Swish-002.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_In_Swish-003.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_In_Swish-004.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_In_Swish-005.wav", g_fSFXVolume, false, false, 5);
}

void CSigrid_State::Play_SFX_HookShot_Out_Launch()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Out_Launch-001.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Out_Launch-002.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Out_Launch-003.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Out_Launch-004.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_HookShot_Out_Launch-005.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Play_SFX_Movement_ChargeJump_Finish()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Finished-001.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Finished-002.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Finished-003.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Finished-004.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Finished-005.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Play_SFX_Movement_ChargeJump_ChargeUp()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_ChargeUp-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_ChargeUp-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_ChargeUp-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_ChargeUp-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_ChargeUp-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_Movement_ChargeJump_Big()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Big-001.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Big-002.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Big-003.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Big-004.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Big-005.wav", g_fSFXVolume, false, false, 4);
}

void CSigrid_State::Play_SFX_Movement_ChargeJump_Small()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Small-001.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Small-002.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Small-003.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Small-004.wav", g_fSFXVolume, false, false, 5);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_ChargeJump_Water_Jump_Small-005.wav", g_fSFXVolume, false, false, 5);
}

void CSigrid_State::Play_SFX_Movement_Dash()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Dodge_Ground-001.wav", g_fSFXVolume, false, true);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Dodge_Ground-002.wav", g_fSFXVolume, false, true);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Dodge_Ground-003.wav", g_fSFXVolume, false, true);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Dodge_Ground-004.wav", g_fSFXVolume, false, true);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Dodge_Ground-005.wav", g_fSFXVolume, false, true);
}

void CSigrid_State::Play_SFX_Movement_Ground_Footstep()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 10.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-001.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-002.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-003.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-004.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-005.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-006.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-007.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-008.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-009.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 9.f && fRand <= 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Footstep-010.wav", g_fSFXVolume, false, true, 1);
}

void CSigrid_State::Play_SFX_Movement_Ground_Jump()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Jump-001.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Jump-002.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Jump-003.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Jump-004.wav", g_fSFXVolume, false, false, 2);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Jump-005.wav", g_fSFXVolume, false, false, 2);
}

void CSigrid_State::Play_SFX_Movement_Ground_Land()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Land-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Land-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Land-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Land-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Grass_Land-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_Movement_Double_Jump()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Jump_DoubleJump-001.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Jump_DoubleJump-002.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Jump_DoubleJump-003.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Jump_DoubleJump-004.wav", g_fSFXVolume, false, false, 4);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Jump_DoubleJump-005.wav", g_fSFXVolume, false, false, 4);
}

void CSigrid_State::Play_SFX_Movement_SnapTurn()
{
}

void CSigrid_State::Play_SFX_Movement_Water_Footstep()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-001.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-002.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-003.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-004.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-005.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-006.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-007.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-008.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-009.wav", g_fSFXVolume, false, true, 1);
	else if (fRand >= 9.f && fRand <= 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Footstep-010.wav", g_fSFXVolume, false, true, 1);
}

void CSigrid_State::Play_SFX_Movement_Water_Jump()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Jump-001.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Jump-002.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Jump-003.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Jump-004.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Jump-005.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Play_SFX_Movement_Water_Land()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_Movement_Water_Land_Hard()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land_Hard-001.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land_Hard-002.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land_Hard-003.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land_Hard-004.wav", g_fSFXVolume, false, false, 3);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Land_Hard-005.wav", g_fSFXVolume, false, false, 3);
}

void CSigrid_State::Play_SFX_Movement_Water_Surf_Boost_Start()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartFast-001.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartFast-002.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartFast-003.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartFast-004.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartFast-005.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Play_SFX_Movement_Water_Surf_Fast_Start()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartSlow-001.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartSlow-002.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartSlow-003.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartSlow-004.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_StartSlow-005.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Play_SFX_Movement_Water_Surf_Slow_Start()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 5.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanStart-001.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanStart-002.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanStart-003.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanStart-004.wav", g_fSFXVolume, false, false, 6);
	else if (fRand >= 4.f && fRand <= 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_Player_Movement_Water_Surf_SuperLeanStart-005.wav", g_fSFXVolume, false, false, 6);
}

void CSigrid_State::Play_Voice_Charge_Attack()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 15.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_01.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_02.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_03.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_04.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_05.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_06.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_07.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_08.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_09.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_10.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_11.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_12.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_13.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 13.f && fRand < 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_14.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 14.f && fRand <= 15.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeAttack_15.wav", g_fVoiceVolume, false, false, 9);
}

void CSigrid_State::Play_Voice_Charge_Jump()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 13.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_01.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_02.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_03.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_04.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_05.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_06.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_07.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_08.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_09.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_10.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_11.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_12.wav", g_fVoiceVolume, false, true, 9);
	else if (fRand >= 12.f && fRand <= 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_ChargeJump_Execute_13.wav", g_fVoiceVolume, false, true, 9);
}

void CSigrid_State::Play_Voice_Attack_Combo_1_3()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 18.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_01.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_02.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_03.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_04.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_05.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_06.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_07.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_08.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_09.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_10.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_11.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_12.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_13.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 13.f && fRand < 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_14.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 14.f && fRand < 15.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_15.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 15.f && fRand < 16.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_16.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 16.f && fRand < 17.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_17.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 17.f && fRand <= 18.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_1_18.wav", g_fVoiceVolume, false, true, 7);
}

void CSigrid_State::Play_Voice_Attack_Combo_2_4()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 18.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_01.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_02.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_03.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_04.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_05.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_06.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_07.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_08.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_09.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_10.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_11.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_12.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_13.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 13.f && fRand < 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_14.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 14.f && fRand < 15.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_15.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 15.f && fRand < 16.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_16.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 16.f && fRand < 17.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_17.wav", g_fVoiceVolume, false, true, 8);
	else if (fRand >= 17.f && fRand <= 18.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Combo_2_18.wav", g_fVoiceVolume, false, true, 8);
}

void CSigrid_State::Play_Voice_Damaged()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 15.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_01.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_02.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_03.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_04.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_05.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_06.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_07.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_08.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_09.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_10.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_11.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_12.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_13.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 13.f && fRand < 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_14.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 14.f && fRand <= 15.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Damage_Major_15.wav", g_fVoiceVolume, false, false, 9);
}

void CSigrid_State::Play_Voice_Dash_Air()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 16.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_01.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_02.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_03.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_04.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_05.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_06.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_07.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_08.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_09.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_10.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_11.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_12.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_13.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 13.f && fRand < 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_14.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 14.f && fRand < 15.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_15.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 15.f && fRand <= 16.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_Air_16.wav", g_fVoiceVolume, false, false, 7);
}

void CSigrid_State::Play_Voice_Dash_Ground()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 12.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_01.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_02.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_03.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_04.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_05.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_06.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_07.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_08.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_09.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_10.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_11.wav", g_fVoiceVolume, false, true, 7);
	else if (fRand >= 11.f && fRand <= 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Dash_New_12.wav", g_fVoiceVolume, false, true, 7);
}

void CSigrid_State::Play_Voice_Grapple_Arrive_Hand()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 13.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_01.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_02.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_03.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_04.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_05.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_06.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_07.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_08.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_09.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_10.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_11.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_12.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 12.f && fRand <= 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Arrive_Hang_13.wav", g_fVoiceVolume, false, false, 9);
}

void CSigrid_State::Play_Voice_Grapple_Launch()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 10.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_01.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_02.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_03.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_04.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_05.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_06.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_07.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_08.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_09.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 9.f && fRand <= 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Grapple_Launch_10.wav", g_fVoiceVolume, false, false, 9);
}

void CSigrid_State::Play_Voice_Attack_Ground_Slam_Intro()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 14.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_01.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_02.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_03.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_04.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_05.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_06.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_07.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_08.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_09.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_10.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_11.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_12.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_13.wav", g_fVoiceVolume, false, false, 9);
	else if (fRand >= 13.f && fRand <= 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Start_14.wav", g_fVoiceVolume, false, false, 9);
}

void CSigrid_State::Play_Voice_Attack_Ground_Slam()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 14.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_01.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_02.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_03.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_04.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_05.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_06.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_07.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_08.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_09.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_10.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_11.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_12.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 12.f && fRand < 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_13.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 13.f && fRand <= 14.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_GroundSlam_Slam_14.wav", g_fVoiceVolume, false, false, 8);
}

void CSigrid_State::Play_Voice_Double_Jump()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 13.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_01.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_02.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_03.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_04.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_05.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_06.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_07.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_08.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_09.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_10.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_11.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_12.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 12.f && fRand <= 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Double_13.wav", g_fVoiceVolume, false, false, 7);
}

void CSigrid_State::Play_Voice_Jump()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 13.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_01.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_02.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_03.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_04.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_05.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_06.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_07.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_08.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_09.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 9.f && fRand < 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_10.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 10.f && fRand < 11.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_11.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 11.f && fRand < 12.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_12.wav", g_fVoiceVolume, false, false, 8);
	else if (fRand >= 12.f && fRand <= 13.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Jump_Single_13.wav", g_fVoiceVolume, false, false, 8);
}

void CSigrid_State::Play_Voice_Land_Hard()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 10.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_01.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_02.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_03.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_04.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_05.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_06.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_07.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_08.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_09.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 9.f && fRand <= 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Hard_10.wav", g_fVoiceVolume, false, false, 7);
}

void CSigrid_State::Play_Voice_Land_Soft()
{
	_float		fRand = CGameUtility::RandomFloat(0.f, 10.f);

	if (fRand >= 0.f && fRand < 1.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_01.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 1.f && fRand < 2.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_02.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 2.f && fRand < 3.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_03.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 3.f && fRand < 4.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_04.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 4.f && fRand < 5.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_05.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 5.f && fRand < 6.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_06.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 6.f && fRand < 7.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_07.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 7.f && fRand < 8.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_08.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 8.f && fRand < 9.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_09.wav", g_fVoiceVolume, false, false, 7);
	else if (fRand >= 9.f && fRand <= 10.f)
		CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Land_Soft_10.wav", g_fVoiceVolume, false, false, 7);
}

void CSigrid_State::Play_Voice_Running()
{
	CGameInstance::GetInstance()->Play_Sound(L"SFX_ActionVO_Sigrid_Running_Loop_04.wav", g_fVoiceVolume, false, false, 9);
}

void CSigrid_State::Move(_double dTimeDelta, CTransform::DIRECTION eDir, MOVETYPE eType)
{
	if (eDir == CTransform::DIR_END || eType == MOVE_END)
		return;

	if (eDir == CTransform::DIR_LOOK)
	{
		m_pTransformCom->Go_Straight(dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
		return;
	}

	_matrix	matCamWorld = XMLoadFloat4x4(&m_pCamera->Get_WorldMatrix());

	_vector	vCamRight = matCamWorld.r[0];
	vCamRight = XMVectorSet(XMVectorGetX(vCamRight), 0.f, XMVectorGetZ(vCamRight), 0.f);
	_vector	vCamLook = matCamWorld.r[2];
	vCamLook = XMVectorSet(XMVectorGetX(vCamLook), 0.f, XMVectorGetZ(vCamLook), 0.f);
	_vector	vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPlayerLook = XMVectorSet(XMVectorGetX(vPlayerLook), 0.f, XMVectorGetZ(vPlayerLook), 0.f);

	_float4	vDir;

	switch (eDir)
	{
	case CTransform::DIR_W:
		vDir = XMVector3Normalize(matCamWorld.r[2]);
		break;

	case CTransform::DIR_A:
		vDir = XMVector3Normalize(matCamWorld.r[0]) * -1.f;
		break;

	case CTransform::DIR_S:
		vDir = XMVector3Normalize(matCamWorld.r[2]) * -1.f;
		break;

	case CTransform::DIR_D:
		vDir = XMVector3Normalize(matCamWorld.r[0]);
		break;

	case CTransform::DIR_WA:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_WD:
		vDir = XMVector3Normalize(matCamWorld.r[2] + matCamWorld.r[0]);
		break;

	case CTransform::DIR_SA:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0] * -1.f);
		break;

	case CTransform::DIR_SD:
		vDir = XMVector3Normalize(matCamWorld.r[2] * -1.f + matCamWorld.r[0]);
		break;
	}

	vDir.y = 0.f;

	if (eType == MOVE_STATIC)
	{
		_float3	vScale = m_pTransformCom->Get_Scale();

		_vector	vLook = XMVector3Normalize(vDir) * vScale.z;
		_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
		_vector	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
		m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
	else if (eType == MOVE_DYNAMIC)
	{
		_float		fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMVector3Normalize(vPlayerLook))));;
		if (isnan(fAngle) != 0)
		{
			m_pTransformCom->Go_Straight(dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
			return;
		}

		_float		fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamRight)));

		if (fDir > 0.f)	/* 카메라 Look 기준 오른쪽을 바라보고 있음. */
		{
			fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamLook)));

			if (fDir > 0.f)	/* 카메라 Right 기준 앞 쪽을 바라보고 있음. 1사분면 */
			{
				fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamRight + vCamLook * -1.f)));

				if (fDir < 0.f)
				{
					switch (eDir)
					{
					case CTransform::DIR_W:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_A:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WA:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SA:
						fAngle *= -1.f;
						break;
					}
				}
				else
				{
					switch (eDir)
					{
					case CTransform::DIR_W:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_A:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WA:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WD:
						fAngle *= -1.f;
						break;
					}
				}
			}
			else /* 카메라 Right 기준 뒤 쪽을 바라보고 있음. 4사분면 */
			{
				fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamRight * -1.f + vCamLook * -1.f)));

				if (fDir < 0.f)
				{
					switch (eDir)
					{
					case CTransform::DIR_W:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_D:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WA:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WD:
						fAngle *= -1.f;
						break;
					}
				}
				else
				{
					switch (eDir)
					{
					case CTransform::DIR_W:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_D:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WD:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SD:
						fAngle *= -1.f;
						break;
					}
				}
			}
		}
		else	/* 카메라 Look 기준 왼쪽을 바라보고 있음. */
		{
			fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamLook)));

			if (fDir > 0.f)	/* 카메라 Right 기준 앞 쪽을 바라보고 있음. 2사분면 */
			{
				fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamRight + vCamLook)));

				if (fDir < 0.f)
				{
					switch (eDir)
					{
					case CTransform::DIR_A:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_S:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SA:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SD:
						fAngle *= -1.f;
						break;
					}
				}
				else
				{
					switch (eDir)
					{
					case CTransform::DIR_A:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_S:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WA:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SA:
						fAngle *= -1.f;
						break;
					}
				}
			}
			else /* 카메라 Right 기준 뒤 쪽을 바라보고 있음. 3사분면 */
			{
				fDir = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(vCamRight * -1.f + vCamLook)));

				if (fDir < 0.f)
				{
					switch (eDir)
					{
					case CTransform::DIR_S:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_D:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_WD:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SD:
						fAngle *= -1.f;
						break;
					}
				}
				else
				{
					switch (eDir)
					{
					case CTransform::DIR_S:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_D:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SA:
						fAngle *= -1.f;
						break;

					case CTransform::DIR_SD:
						fAngle *= -1.f;
						break;
					}
				}
			}
		}
			
		if (dTimeDelta != 0.0)
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle * dTimeDelta * m_pPlayer->m_fInertia);
		else
			m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), fAngle);
	}

	if (eType == MOVE_TURNBACK)
		m_pTransformCom->Go_BackWard(dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
	else
		m_pTransformCom->Go_Straight(dTimeDelta, m_pPlayer->m_pNavigationCom[m_pPlayer->m_eCurNavigation]);
}

CTransform::DIRECTION CSigrid_State::DetectDirectionInput()
{
	CTransform::DIRECTION		eDir = CTransform::DIR_END;

	if (KeyInput_W())
		eDir = CTransform::DIR_W;
	if (KeyInput_A())
		eDir = CTransform::DIR_A;
	if (KeyInput_S())
		eDir = CTransform::DIR_S;
	if (KeyInput_D())
		eDir = CTransform::DIR_D;
	if (KeyInput_WA())
		eDir = CTransform::DIR_WA;
	if (KeyInput_WD())
		eDir = CTransform::DIR_WD;
	if (KeyInput_SA())
		eDir = CTransform::DIR_SA;
	if (KeyInput_SD())
		eDir = CTransform::DIR_SD;

	return eDir;
}

CSigrid_State * CSigrid_State::Create(CSigrid * pPlayer, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform, CStatic_Camera * pCamera)
{
	CSigrid_State*	pInstance = new CSigrid_State;

	if (FAILED(pInstance->Initialize(pPlayer, pStateMachineCom, pModel, pTransform, pCamera)))
	{
		MSG_BOX("Failed to Create : CSigrid_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSigrid_State::Free()
{
}