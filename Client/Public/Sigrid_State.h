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
	void						Tick(_double& dTimeDelta);
	void						Late_Tick(_double& dTimeDelta);
	void						ImGui_RenderProperty();

private:
	HRESULT					SetUp_State_Ground_Idle();
	HRESULT					SetUp_State_Ground_Run();
	HRESULT					SetUp_State_Ground_Boost();
	HRESULT					SetUp_State_Water();
	HRESULT					SetUp_State_Surf();
	HRESULT					SetUp_State_Surf_Boost();
	HRESULT					SetUp_State_Dash();
	HRESULT					SetUp_State_Snap_Turn();
	HRESULT					SetUp_State_Jump();
	HRESULT					SetUp_State_Jump_Double();
	HRESULT					SetUp_State_Jump_Charge();
	HRESULT					SetUp_State_Air();
	HRESULT					SetUp_State_Air_Gliding();
	HRESULT					SetUp_State_Landing();
	HRESULT					SetUp_State_Combat_Combo();
	HRESULT					SetUp_State_Combat_Charge_Attack();
	HRESULT					SetUp_State_Combat_Ground_Slam();
	HRESULT					SetUp_State_Damaged();
	HRESULT					SetUp_State_Grapple_Ground();
	HRESULT					SetUp_State_Grapple_Air();
	HRESULT					SetUp_State_Grapple_Aim();
	HRESULT					SetUp_State_Grapple_Hang();
	HRESULT					SetUp_State_Grapple_Launch();

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

	_double					m_dEffectCloneTimer = 0.0;

	_float	m_fTestLeft = 0.022f;
	_float m_fTestRight = 0.0045f;

private:		/* State Start */
	void	Start_Ground_Idle(_double dTimeDelta);
	void	Start_Dash_Into_Idle(_double dTimeDelta);
	void	Start_Ground_Run(_double dTimeDelta);
	void	Start_Ground_Run_Left(_double dTimeDelta);
	void	Start_Ground_Run_Right(_double dTimeDelta);
	void	Start_Dash_Into_Run(_double dTimeDelta);
	void	Start_Ground_Boost(_double dTimeDelta);
	void	Start_Ground_Boost_Left(_double dTimeDelta);
	void	Start_Ground_Boost_Right(_double dTimeDelta);
	void	Start_Ground_Boost_Stop(_double dTimeDelta);
	void	Start_Water_Idle(_double dTimeDelta);
	void	Start_Water_Run(_double dTimeDelta);
	void	Start_Water_Run_Left(_double dTimeDelta);
	void	Start_Water_Run_Right(_double dTimeDelta);
	void	Start_Water_Braking(_double dTimeDelta);
	void	Start_Water_Diving_Exit(_double dTimeDelta);
	void	Start_Water_Diving(_double dTimeDelta);
	void	Start_Surf_Intro(_double dTimeDelta);
	void	Start_Surf(_double dTimeDelta);
	void	Start_Surf_Left(_double dTimeDelta);
	void	Start_Surf_Right(_double dTimeDelta);
	void	Start_Surf_Fast_Intro(_double dTimeDelta);
	void	Start_Surf_Fast(_double dTimeDelta);
	void	Start_Surf_Fast_Left(_double dTimeDelta);
	void	Start_Surf_Fast_Right(_double dTimeDelta);
	void	Start_Surf_Boost(_double dTimeDelta);
	void	Start_Surf_Boost_Left(_double dTimeDelta);
	void	Start_Surf_Boost_Right(_double dTimeDelta);
	void	Start_Jump(_double dTimeDelta);
	void	Start_Jump_Double(_double dTimeDelta);
	void	Start_Jump_Charging(_double dTimeDelta);
	void	Start_Jump_Charging_Left(_double dTimeDelta);
	void	Start_Jump_Charging_Right(_double dTimeDelta);
	void	Start_Jump_Charged1_60(_double dTimeDelta);
	void	Start_Jump_Charged1_90(_double dTimeDelta);
	void	Start_Jump_Charged2_60(_double dTimeDelta);
	void	Start_Jump_Charged2_90(_double dTimeDelta);
	void	Start_Dash_Into_Air(_double dTimeDelta);
	void	Start_Dash_Into_Water_Idle(_double dTimeDelta);
	void	Start_Dash_Into_Surf(_double dTimeDelta);
	void	Start_Dash_Into_Surf_Fast(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Idle(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Run(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta);
	void	Start_Snap_Turn_Ground_Boost(_double dTimeDelta);
	void	Start_Air(_double dTimeDelta);
	void	Start_Air_Ascend_60(_double dTimeDelta);
	void	Start_Air_Ascend_90(_double dTimeDelta);
	void	Start_Air_Descend_60(_double dTimeDelta);
	void	Start_Air_Descend_90(_double dTimeDelta);
	void	Start_Air_Descend_Fast_60(_double dTimeDelta);
	void	Start_Air_Descend_Fast_90(_double dTimeDelta);
	void	Start_Air_Gliding(_double dTimeDelta);
	void	Start_Air_Gliding_Left(_double dTimeDelta);
	void	Start_Air_Gliding_Right(_double dTimeDelta);
	void	Start_Air_Gliding_Left_Turn(_double dTimeDelta);
	void	Start_Air_Gliding_Right_Turn(_double dTimeDelta);
	void	Start_Landing_Ground_Into_Idle(_double dTimeDelta);
	void	Start_Landing_Ground_Into_Run(_double dTimeDelta);
	void	Start_Landing_Slope(_double dTimeDelta);
	void	Start_Landing_Surf_Fast(_double dTimeDelta);
	void	Start_Landing_Surf(_double dTimeDelta);
	void	Start_Landing_Water(_double dTimeDelta);
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
	void	Start_Combat_Charge_Attack_Into_Water_Idle(_double dTimeDelta);
	void	Start_Combat_Charge_Attack_Into_Water_Run(_double dTimeDelta);
	void	Start_Combat_Charge_Attack_Into_Surf(_double dTimeDelta);
	void	Start_Combat_Charge_Attack_Into_Surf_Fast(_double dTimeDelta);
	void	Start_Combat_Ground_Slam_Intro(_double dTimeDelta);
	void	Start_Combat_Ground_Slam_Fall(_double dTimeDelta);
	void	Start_Combat_Ground_Slam_Hit(_double dTimeDelta);
	void	Start_Damage_Death_Air_Fall_Ground(_double dTimeDelta);
	void	Start_Damage_Death_Air_Fall_Water(_double dTimeDelta);
	void	Start_Damage_Death_Air_Fall(_double dTimeDelta);
	void	Start_Damage_Death_Air(_double dTimeDelta);
	void	Start_Damage_Hit_Air(_double dTimeDelta);
	void	Start_Damage_Hit_Idle(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Fast(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Fast_45(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Fast_90(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Fast_Negative_45(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Fast_Negative_90(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Slow(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Slow_45(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Slow_90(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Slow_Negative_45(_double dTimeDelta);
	void	Start_Grapple_Ground_Fire_Slow_Negative_90(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Fast(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Fast_45(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Fast_90(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Fast_Negative_45(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Fast_Negative_90(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Slow(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Slow_45(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Slow_90(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Slow_Negative_45(_double dTimeDelta);
	void	Start_Grapple_Air_Fire_Slow_Negative_90(_double dTimeDelta);
	void	Start_Grapple_Ground_Aim(_double dTimeDelta);
	void	Start_Grapple_Ground_Aim_Down(_double dTimeDelta);
	void	Start_Grapple_Ground_Aim_Up(_double dTimeDelta);
	void	Start_Grapple_Air_Aim(_double dTimeDelta);
	void	Start_Grapple_Air_Aim_Down(_double dTimeDelta);
	void	Start_Grapple_Air_Aim_Up(_double dTimeDelta);
	void	Start_Grapple_Hang_Intro(_double dTimeDelta);
	void	Start_Grapple_Hang(_double dTimeDelta);
	void	Start_Grapple_Launch_Flip(_double dTimeDelta);
	void	Start_Grapple_Launch_Ready(_double dTimeDelta);
	void	Start_Grapple_Launch(_double dTimeDelta);
	void	Start_Grapple_Launch_45(_double dTimeDelta);
	void	Start_Grapple_Launch_90(_double dTimeDelta);
	void	Start_Grapple_Launch_Negative_45(_double dTimeDelta);
	void	Start_Grapple_Launch_Negative_90(_double dTimeDelta);

private:		/* State Tick */
	void	Tick_Ground_Idle(_double dTimeDelta);
	void	Tick_Dash_Into_Idle(_double dTimeDelta);
	void	Tick_Ground_Run(_double dTimeDelta);
	void	Tick_Ground_Run_Left(_double dTimeDelta);
	void	Tick_Ground_Run_Right(_double dTimeDelta);
	void	Tick_Dash_Into_Run(_double dTimeDelta);
	void	Tick_Ground_Boost(_double dTimeDelta);
	void	Tick_Ground_Boost_Left(_double dTimeDelta);
	void	Tick_Ground_Boost_Right(_double dTimeDelta);
	void	Tick_Ground_Boost_Stop(_double dTimeDelta);
	void	Tick_Water_Idle(_double dTimeDelta);
	void	Tick_Water_Run(_double dTimeDelta);
	void	Tick_Water_Run_Left(_double dTimeDelta);
	void	Tick_Water_Run_Right(_double dTimeDelta);
	void	Tick_Water_Braking(_double dTimeDelta);
	void	Tick_Water_Diving_Exit(_double dTimeDelta);
	void	Tick_Water_Diving(_double dTimeDelta);
	void	Tick_Surf_Intro(_double dTimeDelta);
	void	Tick_Surf(_double dTimeDelta);
	void	Tick_Surf_Left(_double dTimeDelta);
	void	Tick_Surf_Right(_double dTimeDelta);
	void	Tick_Surf_Fast_Intro(_double dTimeDelta);
	void	Tick_Surf_Fast(_double dTimeDelta);
	void	Tick_Surf_Fast_Left(_double dTimeDelta);
	void	Tick_Surf_Fast_Right(_double dTimeDelta);
	void	Tick_Surf_Boost(_double dTimeDelta);
	void	Tick_Surf_Boost_Left(_double dTimeDelta);
	void	Tick_Surf_Boost_Right(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Idle(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Run(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta);
	void	Tick_Snap_Turn_Ground_Boost(_double dTimeDelta);
	void	Tick_Jump(_double dTimeDelta);
	void	Tick_Jump_Double(_double dTimeDelta);
	void	Tick_Jump_Charging(_double dTimeDelta);
	void	Tick_Jump_Charging_Left(_double dTimeDelta);
	void	Tick_Jump_Charging_Right(_double dTimeDelta);
	void	Tick_Jump_Charged1_60(_double dTimeDelta);
	void	Tick_Jump_Charged1_90(_double dTimeDelta);
	void	Tick_Jump_Charged2_60(_double dTimeDelta);
	void	Tick_Jump_Charged2_90(_double dTimeDelta);
	void	Tick_Dash_Into_Air(_double dTimeDelta);
	void	Tick_Dash_Into_Water_Idle(_double dTimeDelta);
	void	Tick_Dash_Into_Surf(_double dTimeDelta);
	void	Tick_Dash_Into_Surf_Fast(_double dTimeDelta);
	void	Tick_Air(_double dTimeDelta);
	void	Tick_Air_Ascend_60(_double dTimeDelta);
	void	Tick_Air_Ascend_90(_double dTimeDelta);
	void	Tick_Air_Descend_60(_double dTimeDelta);
	void	Tick_Air_Descend_90(_double dTimeDelta);
	void	Tick_Air_Descend_Fast_60(_double dTimeDelta);
	void	Tick_Air_Descend_Fast_90(_double dTimeDelta);
	void	Tick_Air_Gliding(_double dTimeDelta);
	void	Tick_Air_Gliding_Left(_double dTimeDelta);
	void	Tick_Air_Gliding_Right(_double dTimeDelta);
	void	Tick_Air_Gliding_Left_Turn(_double dTimeDelta);
	void	Tick_Air_Gliding_Right_Turn(_double dTimeDelta);
	void	Tick_Landing_Ground_Into_Idle(_double dTimeDelta);
	void	Tick_Landing_Ground_Into_Run(_double dTimeDelta);
	void	Tick_Landing_Slope(_double dTimeDelta);
	void	Tick_Landing_Surf_Fast(_double dTimeDelta);
	void	Tick_Landing_Surf(_double dTimeDelta);
	void	Tick_Landing_Water(_double dTimeDelta);
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
	void	Tick_Combat_Charge_Attack_Into_Water_Idle(_double dTimeDelta);
	void	Tick_Combat_Charge_Attack_Into_Water_Run(_double dTimeDelta);
	void	Tick_Combat_Charge_Attack_Into_Surf(_double dTimeDelta);
	void	Tick_Combat_Charge_Attack_Into_Surf_Fast(_double dTimeDelta);
	void	Tick_Combat_Ground_Slam_Intro(_double dTimeDelta);
	void	Tick_Combat_Ground_Slam_Fall(_double dTimeDelta);
	void	Tick_Combat_Ground_Slam_Hit(_double dTimeDelta);
	void	Tick_Damage_Death_Air_Fall_Ground(_double dTimeDelta);
	void	Tick_Damage_Death_Air_Fall_Water(_double dTimeDelta);
	void	Tick_Damage_Death_Air_Fall(_double dTimeDelta);
	void	Tick_Damage_Death_Air(_double dTimeDelta);
	void	Tick_Damage_Hit_Air(_double dTimeDelta);
	void	Tick_Damage_Hit_Idle(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Fast(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Fast_45(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Fast_90(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Fast_Negative_45(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Fast_Negative_90(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Slow(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Slow_45(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Slow_90(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Slow_Negative_45(_double dTimeDelta);
	void	Tick_Grapple_Ground_Fire_Slow_Negative_90(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Fast(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Fast_45(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Fast_90(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Fast_Negative_45(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Fast_Negative_90(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Slow(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Slow_45(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Slow_90(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Slow_Negative_45(_double dTimeDelta);
	void	Tick_Grapple_Air_Fire_Slow_Negative_90(_double dTimeDelta);
	void	Tick_Grapple_Ground_Aim(_double dTimeDelta);
	void	Tick_Grapple_Ground_Aim_Down(_double dTimeDelta);
	void	Tick_Grapple_Ground_Aim_Up(_double dTimeDelta);
	void	Tick_Grapple_Air_Aim(_double dTimeDelta);
	void	Tick_Grapple_Air_Aim_Down(_double dTimeDelta);
	void	Tick_Grapple_Air_Aim_Up(_double dTimeDelta);
	void	Tick_Grapple_Hang_Intro(_double dTimeDelta);
	void	Tick_Grapple_Hang(_double dTimeDelta);
	void	Tick_Grapple_Launch_Flip(_double dTimeDelta);
	void	Tick_Grapple_Launch(_double dTimeDelta);
	void	Tick_Grapple_Launch_45(_double dTimeDelta);
	void	Tick_Grapple_Launch_90(_double dTimeDelta);
	void	Tick_Grapple_Launch_Negative_45(_double dTimeDelta);
	void	Tick_Grapple_Launch_Negative_90(_double dTimeDelta);

private:		/* State End */
	void	End_Ground_Idle(_double dTimeDelta);
	void	End_Dash_Into_Idle(_double dTimeDelta);
	void	End_Ground_Run(_double dTimeDelta);
	void	End_Ground_Run_Left(_double dTimeDelta);
	void	End_Ground_Run_Right(_double dTimeDelta);
	void	End_Dash_Into_Run(_double dTimeDelta);
	void	End_Ground_Boost(_double dTimeDelta);
	void	End_Ground_Boost_Left(_double dTimeDelta);
	void	End_Ground_Boost_Right(_double dTimeDelta);
	void	End_Ground_Boost_Stop(_double dTimeDelta);
	void	End_Water_Idle(_double dTimeDelta);
	void	End_Water_Run(_double dTimeDelta);
	void	End_Water_Run_Left(_double dTimeDelta);
	void	End_Water_Run_Right(_double dTimeDelta);
	void	End_Water_Braking(_double dTimeDelta);
	void	End_Water_Diving_Exit(_double dTimeDelta);
	void	End_Water_Diving(_double dTimeDelta);
	void	End_Surf_Intro(_double dTimeDelta);
	void	End_Surf(_double dTimeDelta);
	void	End_Surf_Left(_double dTimeDelta);
	void	End_Surf_Right(_double dTimeDelta);
	void	End_Surf_Fast_Intro(_double dTimeDelta);
	void	End_Surf_Fast(_double dTimeDelta);
	void	End_Surf_Fast_Left(_double dTimeDelta);
	void	End_Surf_Fast_Right(_double dTimeDelta);
	void	End_Surf_Boost(_double dTimeDelta);
	void	End_Surf_Boost_Left(_double dTimeDelta);
	void	End_Surf_Boost_Right(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Idle(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Run_Into_Idle(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Run(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Boost_Into_Idle(_double dTimeDelta);
	void	End_Snap_Turn_Ground_Boost(_double dTimeDelta);
	void	End_Jump(_double dTimeDelta);
	void	End_Jump_Double(_double dTimeDelta);
	void	End_Jump_Charging(_double dTimeDelta);
	void	End_Jump_Charging_Left(_double dTimeDelta);
	void	End_Jump_Charging_Right(_double dTimeDelta);
	void	End_Jump_Charged1_60(_double dTimeDelta);
	void	End_Jump_Charged1_90(_double dTimeDelta);
	void	End_Jump_Charged2_60(_double dTimeDelta);
	void	End_Jump_Charged2_90(_double dTimeDelta);
	void	End_Dash_Into_Air(_double dTimeDelta);
	void	End_Dash_Into_Water_Idle(_double dTimeDelta);
	void	End_Dash_Into_Surf(_double dTimeDelta);
	void	End_Dash_Into_Surf_Fast(_double dTimeDelta);
	void	End_Air(_double dTimeDelta);
	void	End_Air_Ascend_60(_double dTimeDelta);
	void	End_Air_Ascend_90(_double dTimeDelta);
	void	End_Air_Descend_60(_double dTimeDelta);
	void	End_Air_Descend_90(_double dTimeDelta);
	void	End_Air_Descend_Fast_60(_double dTimeDelta);
	void	End_Air_Descend_Fast_90(_double dTimeDelta);
	void	End_Air_Gliding(_double dTimeDelta);
	void	End_Air_Gliding_Left(_double dTimeDelta);
	void	End_Air_Gliding_Right(_double dTimeDelta);
	void	End_Air_Gliding_Left_Turn(_double dTimeDelta);
	void	End_Air_Gliding_Right_Turn(_double dTimeDelta);
	void	End_Landing_Ground_Into_Idle(_double dTimeDelta);
	void	End_Landing_Ground_Into_Run(_double dTimeDelta);
	void	End_Landing_Slope(_double dTimeDelta);
	void	End_Landing_Surf_Fast(_double dTimeDelta);
	void	End_Landing_Surf(_double dTimeDelta);
	void	End_Landing_Water(_double dTimeDelta);
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
	void	End_Combat_Charge_Attack_Into_Water_Idle(_double dTimeDelta);
	void	End_Combat_Charge_Attack_Into_Water_Run(_double dTimeDelta);
	void	End_Combat_Charge_Attack_Into_Surf(_double dTimeDelta);
	void	End_Combat_Charge_Attack_Into_Surf_Fast(_double dTimeDelta);
	void	End_Combat_Ground_Slam_Intro(_double dTimeDelta);
	void	End_Combat_Ground_Slam_Fall(_double dTimeDelta);
	void	End_Combat_Ground_Slam_Hit(_double dTimeDelta);
	void	End_Damage_Death_Air_Fall_Ground(_double dTimeDelta);
	void	End_Damage_Death_Air_Fall_Water(_double dTimeDelta);
	void	End_Damage_Death_Air_Fall(_double dTimeDelta);
	void	End_Damage_Death_Air(_double dTimeDelta);
	void	End_Damage_Hit_Air(_double dTimeDelta);
	void	End_Damage_Hit_Idle(_double dTimeDelta);
	void	End_Grapple_Ground_Fire(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Fast(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Fast_45(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Fast_90(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Fast_Negative_45(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Fast_Negative_90(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Slow(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Slow_45(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Slow_90(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Slow_Negative_45(_double dTimeDelta);
	void	End_Grapple_Ground_Fire_Slow_Negative_90(_double dTimeDelta);
	void	End_Grapple_Air_Fire(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Fast(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Fast_45(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Fast_90(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Fast_Negative_45(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Fast_Negative_90(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Slow(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Slow_45(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Slow_90(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Slow_Negative_45(_double dTimeDelta);
	void	End_Grapple_Air_Fire_Slow_Negative_90(_double dTimeDelta);
	void	End_Grapple_Ground_Aim(_double dTimeDelta);
	void	End_Grapple_Ground_Aim_Down(_double dTimeDelta);
	void	End_Grapple_Ground_Aim_Up(_double dTimeDelta);
	void	End_Grapple_Air_Aim(_double dTimeDelta);
	void	End_Grapple_Air_Aim_Down(_double dTimeDelta);
	void	End_Grapple_Air_Aim_Up(_double dTimeDelta);
	void	End_Grapple_Hang_Intro(_double dTimeDelta);
	void	End_Grapple_Hang(_double dTimeDelta);
	void	End_Grapple_Launch_Flip(_double dTimeDelta);
	void	End_Grapple_Launch(_double dTimeDelta);
	void	End_Grapple_Launch_45(_double dTimeDelta);
	void	End_Grapple_Launch_90(_double dTimeDelta);
	void	End_Grapple_Launch_Negative_45(_double dTimeDelta);
	void	End_Grapple_Launch_Negative_90(_double dTimeDelta);

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
	_bool	KeyInput_Space_Progress();
	_bool	KeyInput_Shift();
	_bool	KeyInput_LCTRL();

	_bool	KeyDown_F();
	_bool	KeyDown_Space();
	_bool	KeyDown_Space_Progress();
	_bool	KeyDown_LCTRL();
	_bool	KeyDown_Escape();

	_bool	KeyDoubleDown_Space();
	_bool	KeyDoubleDown_Space_Progress();

	_bool	KeyUp_A();
	_bool	KeyUp_D();
	_bool	KeyUp_Space();
	_bool	KeyUp_Shift();

	_bool	KeyCharge_Space();
	_bool	KeyCharge_Space_Progress();

	_bool	MouseDown_LB();
	_bool	MousePressing_RB();
	_bool	MouseDown_RB();
	_bool	MouseUp_RB();

	_bool	KeyDown_Space_While_Dash_Into_Air();
	_bool	KeyCharge_Space_Jump_Charge_00();
	_bool	KeyCharge_Space_Jump_Charge_00_With_Dir();
	_bool	KeyCharge_Space_Jump_Charge_05();
	_bool	KeyCharge_Space_Jump_Charge_05_With_Dir();
	_bool	KeyCharge_Space_Jump_Charge_10();
	_bool	KeyCharge_Space_Jump_Charge_10_With_Dir();
	_bool	KeyCharge_Space_Jump_Charge_15();
	_bool	KeyCharge_Space_Jump_Charge_15_With_Dir();
	_bool	KeyCharge_Space_Jump_Charge_20();
	_bool	KeyCharge_Space_Jump_Charge_20_With_Dir();

	_bool	Air0();
	_bool	AirAscend60();
	_bool	AirAscend90();
	_bool	AirDescend_60();
	_bool	AirDescend_90();
	_bool	AirDescendFast();
	_bool	Damaged();
	_bool	SnapGrapple();
	_bool	SnapGrappleFast();
	_bool	SnapGrappleFast45();
	_bool	SnapGrappleFast90();
	_bool	SnapGrappleFastNeg45();
	_bool	SnapGrappleFastNeg90();
	_bool	SnapGrappleSlow();
	_bool	SnapGrappleSlow45();
	_bool	SnapGrappleSlow90();
	_bool	SnapGrappleSlowNeg45();
	_bool	SnapGrappleSlowNeg90();
	_bool	GrappleLaunchAngle0();
	_bool	GrappleLaunchAngle45();
	_bool	GrappleLaunchAngle90();
	_bool	GrappleLaunchAngleNeg45();
	_bool	GrappleLaunchAngleNeg90();
	_bool	Grapple_Done();
	_bool	ReadyLaunch();
	_bool	ReadyHang();
	_bool	Turn_Back();
	_bool	Turn_Back_Left();
	_bool	Turn_Back_Right();
	_bool	Check_SurfTime();
	_bool	Camera_Angle_Coincide();
	_bool	Camera_Angle_Turn_Left();
	_bool	Camera_Angle_Turn_Right();
	_bool	Animation_Finish();
	_bool	Animation_Progress_30();
	_bool	Animation_Progress_30_KeyInput_Direction();
	_bool	Animation_Progress_30_KeyInput_None();
	_bool	Animation_Progress_35();
	_bool	Animation_Progress_40();
	_bool	Animation_Progress_40_KeyInput_Direction();
	_bool	Animation_Progress_40_KeyInput_None();
	_bool	Animation_Progress_50();
	_bool	Animation_Progress_50_KeyInput_Direction();
	_bool	Animation_Progress_50_KeyInput_None();
	_bool	Animation_Progress_60();
	_bool	Animation_Progress_60_KeyInput_Direction();
	_bool	Animation_Progress_60_KeyInput_None();
	_bool	Animation_Progress_70();
	_bool	Animation_Progress_80();
	_bool	Animation_Progress_90();
	_bool	Collision_Ground();
	_bool	Collision_Ground_KeyInput_None();
	_bool	Collision_Ground_KeyInput_Direction();
	_bool	Collision_Ocean_KeyInput_None();
	_bool	Collision_Ocean_KeyInput_Direction();
	_bool	Collision_Ocean_KeyInput_Direction_SurfTime();
	_bool	Collision_Ocean_Boost();
	_bool	IsOnGround();
	_bool	IsOnOcean();
	_bool	IsOnAir();

private:	/* SFX Sound */
	void	Play_SFX_Attack_Charge_Start();
	void	Play_SFX_Attack_Charge_Execute();
	void	Play_SFX_Attack_Charge_Done();
	void	Play_SFX_Attack_GroundSlam_Intro();
	void	Play_SFX_Attack_GroundSlam_Hit();
	void	Play_SFX_Attack_Combo_1_3();
	void	Play_SFX_Attack_Combo_2_4();
	void	Play_SFX_Damaged();
	void	Play_SFX_Hit();
	void	Play_SFX_Movement_Glide_Fly_Start();
	void	Play_SFX_HookShot_AimIn();
	void	Play_SFX_HookShot_Arrive();
	void	Play_SFX_HookShot_In_Swish();
	void	Play_SFX_HookShot_Out_Launch();
	void	Play_SFX_Movement_ChargeJump_Finish();
	void	Play_SFX_Movement_ChargeJump_ChargeUp();
	void	Play_SFX_Movement_ChargeJump_Big();
	void	Play_SFX_Movement_ChargeJump_Small();
	void	Play_SFX_Movement_Dash();
	void	Play_SFX_Movement_Ground_Footstep();
	void	Play_SFX_Movement_Ground_Jump();
	void	Play_SFX_Movement_Ground_Land();
	void	Play_SFX_Movement_Double_Jump();
	void	Play_SFX_Movement_SnapTurn();
	void	Play_SFX_Movement_Water_Footstep();
	void	Play_SFX_Movement_Water_Jump();
	void	Play_SFX_Movement_Water_Land();
	void	Play_SFX_Movement_Water_Land_Hard();
	void	Play_SFX_Movement_Water_Surf_Boost_Start();
	void	Play_SFX_Movement_Water_Surf_Fast_Start();
	void	Play_SFX_Movement_Water_Surf_Slow_Start();

private:	/* Voice Sound */
	void	Play_Voice_Charge_Attack();
	void	Play_Voice_Charge_Jump();
	void	Play_Voice_Attack_Combo_1_3();
	void	Play_Voice_Attack_Combo_2_4();
	void	Play_Voice_Damaged();
	void	Play_Voice_Dash_Air();
	void	Play_Voice_Dash_Ground();
	void	Play_Voice_Grapple_Arrive_Hand();
	void	Play_Voice_Grapple_Launch();
	void	Play_Voice_Attack_Ground_Slam_Intro();
	void	Play_Voice_Attack_Ground_Slam();
	void	Play_Voice_Double_Jump();
	void	Play_Voice_Jump();
	void	Play_Voice_Land_Hard();
	void	Play_Voice_Land_Soft();
	void	Play_Voice_Running();

private:
	void	Move(_double dTimeDelta, CTransform::DIRECTION eDir, MOVETYPE eType = MOVE_DYNAMIC);
	CTransform::DIRECTION	DetectDirectionInput();

public:
	static CSigrid_State*	Create(class CSigrid* pPlayer, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform, class CStatic_Camera* pCamera);
	virtual void				Free() override;
};

END