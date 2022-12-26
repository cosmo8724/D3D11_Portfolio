#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "Transform.h"

BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CModel;
END

BEGIN(Client)

class CSigrid_State final : public CBase
{
public:
	enum MOVETYPE { MOVE_STATIC, MOVE_DYNAMIC, MOVE_TURNBACK, MOVE_END };
	enum ANIMATION {
		/* Grapple */
		GRAPPLE_SPARK_AIR, GRAPPLE_SPARK_GROUND,

		/* Air */
		AIR_ASCEND_60, AIR_ASCEND_90, AIR_DESCEND_60, AIR_DESCEND_90, AIR_DESCEND_FAST_60, AIR_DESCEND_FAST_90,

		/* Air Dive */
		AIR_DIVE_INTRO_60, AIR_DIVE_INTRO_90, AIR_DIVE_INTRO_CANCEL, AIR_DIVE_READY_DOWN, AIR_DIVE_READY_SLOW, AIR_DIVE_READY,
		AIR_DIVING_60, AIR_DIVING_90, AIR_DIVING_CANCEL_45, AIR_DIVING_CANCEL_90, AIR_DIVING_INTO_WATER,

		/* Air Gliding */
		AIR_GLIDING_LEFT_TURN, AIR_GLIDING_LEFT, AIR_GLIDING_RIGHT_TURN, AIR_GLIDING_RIGHT, AIR_GLIDING,

		/* Air */
		AIR,

		/* Combat Charge Attack */
		COMBAT_CHARGE_ATTACK_INTO_IDLE, COMBAT_CHARGE_ATTACK_INTO_RUN, COMBAT_CHARGE_ATTACK_INTO_SURF_FAST, COMBAT_CHARGE_ATTACK_INTO_SURF, COMBAT_CHARGE_ATTACK_INTO_WATER_IDLE,

		/* Combat Combo 1 */
		COMBAT_COMBO1_AIR, COMBAT_COMBO1_INTO_IDLE, COMBAT_COMBO1_INTO_RUN, COMBAT_COMBO1_INTO_SURF,

		/* Combat Combo 2 */
		COMBAT_COMBO2_AIR, COMBAT_COMBO2_INTO_IDLE, COMBAT_COMBO2_INTO_RUN, COMBAT_COMBO2_INTO_SURF,

		/* Combat Combo 3 */
		COMBAT_COMBO3_AIR, COMBAT_COMBO3_INTO_IDLE, COMBAT_COMBO3_INTO_RUN, COMBAT_COMBO3_INTO_SURF,

		/* Combat Combo 4 */
		COMBAT_COMBO4_AIR, COMBAT_COMBO4_INTO_IDLE, COMBAT_COMBO4_INTO_RUN, COMBAT_COMBO4_INTO_SURF,

		/* Combat Ground */
		COMBAT_GROUND_SLAM_FALL, COMBAT_GROUND_SLAM_HIT, COMBAT_GROUND_SLAM_INTRO,

		/* Damaged */
		DAMAGE_DEATH_AIR_FALL_GROUND, DAMAGE_DEATH_AIR_FALL_WATER, DAMAGE_DEATH_AIR_FALL, DAMAGE_DEATH_AIR,
		DAMAGE_HIT_AIR, DAMAGE_HIT_IDLE,

		/* Dash */
		DASH_INTO_AIR, DASH_INTO_IDLE, DASH_INTO_RUN, DASH_INTO_SURF_FAST, DASH_INTO_SURF, DASH_INTO_WATER_IDLE,

		/* Disable Net */
		DISABLE_NET_AIR_ASCEND, DISABLE_NET_AIR_DESCEND, DISABLE_NET_AIR, DISABLE_NET_GROUND_BOOST, DISABLE_NET_GROUND_RUN, DISABLE_NET_GROUND,
		DISABLE_NET_SURF_BOOST, DISABLE_NET_SURF, DISABLE_NET_WATER,

		/* Grapple in Air */
		GRAPPLE_AIR_AIM_DOWN, GRAPPLE_AIR_AIM_UP, GRAPPLE_AIR_AIM,
		GRAPPLE_AIR_FIRE_FAST_45, GRAPPLE_AIR_FIRE_FAST_90, GRAPPLE_AIR_FIRE_FAST_NEGATIVE_45, GRAPPLE_AIR_FIRE_FAST_NEGATIVE_90, GRAPPLE_AIR_FIRE_FAST,
		GRAPPLE_AIR_FIRE_SLOW_45, GRAPPLE_AIR_FIRE_SLOW_90, GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_45, GRAPPLE_AIR_FIRE_SLOW_NEGATIVE_90, GRAPPLE_AIR_FIRE_SLOW,

		/* Grapple Flying */
		GRAPPLE_FLYING_45, GRAPPLE_FLYING_90, GRAPPLE_FLYING_NEGATIVE_45, GRAPPLE_FLYING_NEGATIVE_90, GRAPPLE_FLYING,

		/* Grapple on Ground */
		GRAPPLE_GROUND_AIM_DOWN, GRAPPLE_GROUND_AIM_UP, GRAPPLE_GROUND_AIM,
		GRAPPLE_GROUND_FIRE_FAST_45, GRAPPLE_GROUND_FIRE_FAST_90, GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_45, GRAPPLE_GROUND_FIRE_FAST_NEGATIVE_90, GRAPPLE_GROUND_FIRE_FAST,
		GRAPPLE_GROUND_FIRE_SLOW_45, GRAPPLE_GROUND_FIRE_SLOW_90, GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_45, GRAPPLE_GROUND_FIRE_SLOW_NEGATIVE_90, GRAPPLE_GROUND_FIRE_SLOW,

		/* Grapple Hanging */
		GRAPPLE_HANG_INTRO, GRAPPLE_HANG,

		/* Grapple Launch */
		GRAPPLE_LAUNCH_45, GRAPPLE_LAUNCH_90, GRAPPLE_LAUNCH_FLIP, GRAPPLE_LAUNCH_NEGATIVE_45, GRAPPLE_LAUNCH_NEGATIVE_90, GRAPPLE_LAUNCH,

		/* Grapple Spark Surf */
		GRAPPLE_SPARK_SURF_FAST, GRAPPLE_SPARK_SURF,

		/* Ground Boost */
		GROUND_BOOST_LEFT, GROUND_BOOST_RIGHT, GROUND_BOOST_STOP, GROUND_BOOST,

		/* Ground Idle */
		GROUND_IDLE_25, GROUND_IDLE_45, GROUND_IDLE_NEGATIVE_25, GROUND_IDLE_NEGATIVE_45, GROUND_IDLE,

		/* Ground Run */
		GROUND_RUN_25, GROUND_RUN_45, GROUND_RUN_LEFT, GROUND_RUN_NEGATIVE_25, GROUND_RUN_RIGHT, GROUND_RUN_START, GROUND_RUN_STOP, GROUND_RUN,

		/* Ground Walk */
		GROUND_WALK_25, GROUND_WALK_45, GROUND_WALK_LEFT, GROUND_WALK_NEGATIVE_25, GROUND_WALK_NEGATIVE_45, GROUND_WALK_RIGHT, GROUND_WALK_START,
		GROUND_WALK_STOP_LEFT_FOOT_PLANTED, GROUND_WALK_STOP_LEFT_FOOT, GROUND_WALK_STOP_RIGHT_FOOT_PLANTED, GROUND_WALK_STOP_RIGHT_FOOT, GROUND_WALK,

		/* Jump Charge */
		JUMP_CHARGED1_60, JUMP_CHARGED1_90, JUMP_CHARGED2_60, JUMP_CHARGED2_90,

		/* Jump Charging */
		JUMP_CHARGING_LEFT, JUMP_CHARGING_RIGHT, JUMP_CHARGING,

		/* Jump */
		JUMP_CREST, JUMP_DOUBLE, JUMP,

		/* Landing */
		LANDING_GROUND_INTO_IDLE_25, LANDING_GROUND_INTO_IDLE_NEGATIVE_25, LANDING_GROUND_INTO_IDLE,
		LANDING_GROUND_INTO_RUN_25, LANDING_GROUND_INTO_RUN_NEGATIVE_25, LANDING_GROUND_INTO_RUN,
		LANDING_SLOPE, LANDING_SURF_FAST, LANDING_SURF, LANDING_WATER,

		/* Snap Turn */
		SNAP_TURN_GROUND_BOOST_INTO_IDLE, SNAP_TURN_GROUND_BOOST, SNAP_TURN_GROUND_IDLE, SNAP_TURN_GROUND_RUN_INTO_IDLE, SNAP_TURN_GROUND_RUN,

		/* Strider Kill */
		STRIDER_KILL,

		/* Surf Boost */
		SURF_BOOST_LEFT, SURF_BOOST_RIGHT, SURF_BOOST,

		/* Surf */
		SURF_DOWN, SURF_FAST_DOWN, SURF_FAST_INTRO, SURF_FAST_LEFT, SURF_FAST_RIGHT, SURF_FAST_UP, SURF_FAST,
		SURF_INTRO, SURF_LEFT, SURF_RIGHT, SURF_UP, SURF,

		/* On Water */
		WATER_BRAKING, WATER_DIVING_EXIT, WATER_DIVING, WATER_IDLE,
	};

private:
	CSigrid_State();
	virtual ~CSigrid_State() = default;

public:
	HRESULT					Initialize(class CSigrid* pPlayer, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform, class CStatic_Camera* pCamera);
	void						Tick(_double dTimeDelta);
	void						Late_Tick(_double dTimeDelta);

private:
	HRESULT					SetUp_State_Ground_Idle();
	HRESULT					SetUp_State_Dash_Into_Idle();
	HRESULT					SetUp_State_Ground_Run();
	HRESULT					SetUp_State_Dash_Into_Run();
	HRESULT					SetUp_State_Snap_Turn();
	HRESULT					SetUp_State_Jump();
	HRESULT					SetUp_State_Jump_Double();
	HRESULT					SetUp_State_Dash_Into_Air();
	HRESULT					SetUp_State_Air();
	HRESULT					SetUp_State_Air_Gliding();
	HRESULT					SetUp_State_Combat_Combo();
	HRESULT					SetUp_State_Combat_Charge_Attack();
	HRESULT					SetUp_State_Combat_Ground_Slam();

private:
	class CGameInstance*	m_pGameInstance = nullptr;

private:
	class CSigrid*				m_pPlayer = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	class CStatic_Camera*	m_pCamera = nullptr;

private:
	CTransform::DIRECTION		m_eDir = CTransform::DIR_END;
	CTransform::DIRECTION		m_eLastDir = CTransform::DIR_END;

private:		/* State Start */
	void	Start_Ground_Idle(_double dTimeDelta);
	void	Start_Dash_Into_Idle(_double dTimeDelta);
	void	Start_Ground_Run(_double dTimeDelta);
	void	Start_Dash_Into_Run(_double dTimeDelta);
	void	Start_Jump(_double dTimeDelta);
	void	Start_Jump_Double(_double dTimeDelta);
	void	Start_Dash_Into_Air(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Idle(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Run(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Boost(_double dTimeDelta);
	void	Start_Air(_double dTimeDelta);
	void	Start_Air_Gliding(_double dTimeDelta);
	void	Start_Air_Gliding_Left(_double dTimeDelta);
	void	Start_Air_Gliding_Right(_double dTimeDelta);
	void	Start_Air_Gliding_Left_Turn(_double dTimeDelta);
	void	Start_Air_Gliding_Right_Turn(_double dTimeDelta);
	void	Start_Combat_Combo1_Into_Idle(_double dTimeDelta);
	void	Start_Combat_Combo1_Into_Run(_double dTimeDelta);
	void	Start_Combat_Combo1_Into_Surf(_double dTimeDelta);
	void	Start_Combat_Combo1_Air(_double dTimeDelta);
	void	Start_Combat_Combo2_Into_Idle(_double dTimeDelta);
	void	Start_Combat_Combo2_Into_Run(_double dTimeDelta);
	void	Start_Combat_Combo2_Into_Surf(_double dTimeDelta);
	void	Start_Combat_Combo2_Air(_double dTimeDelta);
	void	Start_Combat_Combo3_Into_Idle(_double dTimeDelta);
	void	Start_Combat_Combo3_Into_Run(_double dTimeDelta);
	void	Start_Combat_Combo3_Into_Surf(_double dTimeDelta);
	void	Start_Combat_Combo3_Air(_double dTimeDelta);
	void	Start_Combat_Combo4_Into_Idle(_double dTimeDelta);
	void	Start_Combat_Combo4_Into_Run(_double dTimeDelta);
	void	Start_Combat_Combo4_Into_Surf(_double dTimeDelta);
	void	Start_Combat_Combo4_Air(_double dTimeDelta);
	void	Start_Combat_Charge_Attack_Into_Idle(_double dTimeDelta);
	void	Start_Combat_Charge_Attack_Into_Run(_double dTimeDelta);
	void	Start_Combat_Ground_Slam_Intro(_double dTimeDelta);
	void	Start_Combat_Ground_Slam_Fall(_double dTimeDelta);
	void	Start_Combat_Ground_Slam_Hit(_double dTimeDelta);


private:		/* State Tick */
	void	Tick_Ground_Idle(_double dTimeDelta);
	void	Tick_Dash_Into_Idle(_double dTimeDelta);
	void	Tick_Ground_Run(_double dTimeDelta);
	void	Tick_Ground_Run_Front(_double dTimeDelta);
	void	Tick_Ground_Run_Left(_double dTimeDelta);
	void	Tick_Ground_Run_Back(_double dTimeDelta);
	void	Tick_Ground_Run_Right(_double dTimeDelta);
	void	Tick_Ground_Run_Front_Left(_double dTimeDelta);
	void	Tick_Ground_Run_Front_Right(_double dTimeDelta);
	void	Tick_Ground_Run_Back_Left(_double dTimeDelta);
	void	Tick_Ground_Run_Back_Right(_double dTimeDelta);
	void	Tick_Dash_Into_Run(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Idle(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Run(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Boost(_double dTimeDelta);
	void	Tick_Jump(_double dTimeDelta);
	void	Tick_Jump_Double(_double dTimeDelta);
	void	Tick_Dash_Into_Air(_double dTimeDelta);
	void	Tick_Air(_double dTimeDelta);
	void	Tick_Air_Gliding(_double dTimeDelta);
	void	Tick_Air_Gliding_Left(_double dTimeDelta);
	void	Tick_Air_Gliding_Right(_double dTimeDelta);
	void	Tick_Air_Gliding_Left_Turn(_double dTimeDelta);
	void	Tick_Air_Gliding_Right_Turn(_double dTimeDelta);
	void	Tick_Combat_Combo1_Into_Idle(_double dTimeDelta);
	void	Tick_Combat_Combo1_Into_Run(_double dTimeDelta);
	void	Tick_Combat_Combo1_Into_Surf(_double dTimeDelta);
	void	Tick_Combat_Combo1_Air(_double dTimeDelta);
	void	Tick_Combat_Combo2_Into_Idle(_double dTimeDelta);
	void	Tick_Combat_Combo2_Into_Run(_double dTimeDelta);
	void	Tick_Combat_Combo2_Into_Surf(_double dTimeDelta);
	void	Tick_Combat_Combo2_Air(_double dTimeDelta);
	void	Tick_Combat_Combo3_Into_Idle(_double dTimeDelta);
	void	Tick_Combat_Combo3_Into_Run(_double dTimeDelta);
	void	Tick_Combat_Combo3_Into_Surf(_double dTimeDelta);
	void	Tick_Combat_Combo3_Air(_double dTimeDelta);
	void	Tick_Combat_Combo4_Into_Idle(_double dTimeDelta);
	void	Tick_Combat_Combo4_Into_Run(_double dTimeDelta);
	void	Tick_Combat_Combo4_Into_Surf(_double dTimeDelta);
	void	Tick_Combat_Combo4_Air(_double dTimeDelta);
	void	Tick_Combat_Charge_Attack_Into_Idle(_double dTimeDelta);
	void	Tick_Combat_Charge_Attack_Into_Run(_double dTimeDelta);
	void	Tick_Combat_Ground_Slam_Intro(_double dTimeDelta);
	void	Tick_Combat_Ground_Slam_Fall(_double dTimeDelta);
	void	Tick_Combat_Ground_Slam_Hit(_double dTimeDelta);

private:		/* State End */
	void	End_Ground_Idle(_double dTimeDelta);
	void	End_Dash_Into_Idle(_double dTimeDelta);
	void	End_Ground_Run(_double dTimeDelta);
	void	End_Ground_Run_Front(_double dTimeDelta);
	void	End_Ground_Run_Left(_double dTimeDelta);
	void	End_Ground_Run_Back(_double dTimeDelta);
	void	End_Ground_Run_Right(_double dTimeDelta);
	void	End_Ground_Run_Front_Left(_double dTimeDelta);
	void	End_Ground_Run_Front_Right(_double dTimeDelta);
	void	End_Ground_Run_Back_Left(_double dTimeDelta);
	void	End_Ground_Run_Back_Right(_double dTimeDelta);
	void	End_Dash_Into_Run(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Idle(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Run(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Boost(_double dTimeDelta);
	void	End_Jump(_double dTimeDelta);
	void	End_Jump_Double(_double dTimeDelta);
	void	End_Dash_Into_Air(_double dTimeDelta);
	void	End_Air(_double dTimeDelta);
	void	End_Air_Gliding(_double dTimeDelta);
	void	End_Air_Gliding_Left(_double dTimeDelta);
	void	End_Air_Gliding_Right(_double dTimeDelta);
	void	End_Air_Gliding_Left_Turn(_double dTimeDelta);
	void	End_Air_Gliding_Right_Turn(_double dTimeDelta);
	void	End_Combat_Combo1_Into_Idle(_double dTimeDelta);
	void	End_Combat_Combo1_Into_Run(_double dTimeDelta);
	void	End_Combat_Combo1_Into_Surf(_double dTimeDelta);
	void	End_Combat_Combo1_Air(_double dTimeDelta);
	void	End_Combat_Combo2_Into_Idle(_double dTimeDelta);
	void	End_Combat_Combo2_Into_Run(_double dTimeDelta);
	void	End_Combat_Combo2_Into_Surf(_double dTimeDelta);
	void	End_Combat_Combo2_Air(_double dTimeDelta);
	void	End_Combat_Combo3_Into_Idle(_double dTimeDelta);
	void	End_Combat_Combo3_Into_Run(_double dTimeDelta);
	void	End_Combat_Combo3_Into_Surf(_double dTimeDelta);
	void	End_Combat_Combo3_Air(_double dTimeDelta);
	void	End_Combat_Combo4_Into_Idle(_double dTimeDelta);
	void	End_Combat_Combo4_Into_Run(_double dTimeDelta);
	void	End_Combat_Combo4_Into_Surf(_double dTimeDelta);
	void	End_Combat_Combo4_Air(_double dTimeDelta);
	void	End_Combat_Charge_Attack_Into_Idle(_double dTimeDelta);
	void	End_Combat_Charge_Attack_Into_Run(_double dTimeDelta);
	void	End_Combat_Ground_Slam_Intro(_double dTimeDelta);
	void	End_Combat_Ground_Slam_Fall(_double dTimeDelta);
	void	End_Combat_Ground_Slam_Hit(_double dTimeDelta);

private:		/* State Changer*/
	_bool	KeyInput_None();
	_bool	KeyInput_Direction();
	_bool	KeyInput_Direction_Progress();
	_bool	KeyInput_W();
	_bool	KeyInput_A();
	_bool	KeyInput_S();
	_bool	KeyInput_D();
	_bool	KeyInput_WA();
	_bool	KeyInput_WD();
	_bool	KeyInput_SA();
	_bool	KeyInput_SD();
	_bool	KeyInput_WtoS();
	_bool	KeyInput_StoW();
	_bool	KeyInput_AtoD();
	_bool	KeyInput_DtoA();
	_bool	KeyInput_Space();
	_bool	KeyInput_Shift();
	_bool	KeyInput_LCTRL();

	_bool	KeyDown_F();
	_bool	KeyDown_Space();
	_bool	KeyDown_Space_Progress();
	_bool	KeyDown_LCTRL();
	_bool	KeyDown_Escape();

	_bool	KeyDoubleDown_Space();
	_bool	KeyDoubleDown_Space_Progress();

	_bool	KeyUp_Space();

	_bool	KeyCharge_Space();
	_bool	KeyCharge_Space_Progress();

	_bool	MouseDown_LB();
	_bool	MouseDown_RB();

	_bool	KeyDown_Space_While_Dash_Into_Air();

	_bool	Turn_Back();
	_bool	Animation_Finish();
	_bool	Collision_Ground();

private:
	void	Move(_double dTimeDelta, CTransform::DIRECTION eDir, MOVETYPE eType = MOVE_DYNAMIC);
	CTransform::DIRECTION	DetectDirectionInput();

public:
	static CSigrid_State*	Create(class CSigrid* pPlayer, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform, class CStatic_Camera* pCamera);
	virtual void				Free() override;
};

END