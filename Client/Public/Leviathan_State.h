#pragma once
#include "Base.h"
#include "Client_Define.h"
#include "Transform.h"

BEGIN(Engine)
class CGameInstance;
class CStateMachine;
class CModel;
END

BEGIN(Client)

class CLeviathan_State final : public CBase
{
public:
	enum ANIMATION {
		APPEAR, PHASE_CHANGE,
		ATT_BITE, ATT_LIGHTNING_ST, ATT_LIGHTNING_LP, ATT_LIGHTNING_ED,
		ATT_MOVE_1, ATT_MOVE_2, ATT_MOVE_3, ATT_SPIN, ATT_SPLASH, ATT_TAIL_WHIP, ATT_WATER_BALL,
		ATT_WATER_BEAM_ST, ATT_WATER_BEAM_RIGHT_LP, ATT_WATER_BEAM_RIGHT_ED, ATT_WATER_BEAM_LEFT_LP, ATT_WATER_BEAM_LEFT_ED,
		GROGGY_UP, ATT_WATER_TORNADO, ATT_WING,
		DMG_BACK, DMG_DOWN_ST, DMG_DOWN_LP, DMG_DOWN_ED,
		DIE_1, DIE_2, DIE_3,
		GROGGY_DOWN, DMG_LEFT, DMG_RIGHT, DIE_DOWN,
		FLY_MOVE_1, FLY_MOVE_2,
		IDLE_ST, IDLE_LP, IDLE_ED,
		TURN_RIGHT_180, TURN_RIGHT_90, TURN_LEFT_180, TURN_LEFT_90,
		WARP_1, WARP_2
	};

	enum POSITION { POS_A, POS_B, POS_C, POS_D, POS_E, POS_F, POS_G, POS_H, POS_END, POS_PLAYER };

	enum PHASE { PHASE_1, PHASE_2, PHASE_END };

	enum PHASE_1_PATTERN {
		BITE, LIGHTNING, /*MOVE, */SPLASH, TAIL_WHIP, WATER_BALL, PHASE_1_PATTERN_END
	};

	enum PHASE_2_PATTERN {
		SPIN = 5, WATER_BEAM, /*WATER_TORNADO, WING,*/ PHASE_2_PATTERN_END
	};

private:
	CLeviathan_State();
	virtual ~CLeviathan_State() = default;

public:
	HRESULT					Initialize(DEVICE pDevice, DEVICE_CONTEXT pContext, class CLeviathan* pLeviathan, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	void						Tick(_double dTimeDelta);
	void						Late_Tick(_double dTimeDelta);

private:
	HRESULT					SetUp_State_Idle();
	HRESULT					SetUp_State_Warp();
	HRESULT					SetUp_State_Attack_Phase_1();
	HRESULT					SetUp_State_Attack_Phase_2();
	HRESULT					SetUp_State_Damaged();
	HRESULT					SetUp_State_Die();
	HRESULT					SetUp_State_FlyMove();
	HRESULT					SetUp_State_Turn();

private:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;
	class CGameInstance*	m_pGameInstance = nullptr;

private:
	class CLeviathan*		m_pLeviathan = nullptr;
	CStateMachine*			m_pStateMachineCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:
	POSITION					m_eCurrentPos = POS_A;
	PHASE						m_ePhase = PHASE_1;
	_uint						m_iCurrentPattern = (_uint)PHASE_2_PATTERN_END;
	_uint						m_iLastPattern = (_uint)PHASE_2_PATTERN_END;
	_bool						m_bGroggy = false;

	_uint						m_iIdleFinishCount = 0;
	_uint						m_iLightningCount = 0;
	_uint						m_iLightningCount2 = 0;
	_bool						m_bLightning = false;
	_uint						m_iLightningPattern = 0;
	_double					m_dLightningTime = 0.0;

	_double					m_dHPRecoverTime = 0.0;

private:
	void						Start_Appear(_double dTimeDelta);
	void						Start_Idle_Start(_double dTimeDelta);
	void						Start_Idle_Loop(_double dTimeDelta);
	void						Start_Idle_End(_double dTimeDelta);
	void						Start_Warp_1(_double dTimeDelta);
	void						Start_Warp_2(_double dTimeDelta);
	void						Start_Attack_Bite(_double dTimeDelta);
	void						Start_Attack_Lightning_Start(_double dTimeDelta);
	void						Start_Attack_Lightning_Loop(_double dTimeDelta);
	void						Start_Attack_Lightning_End(_double dTimeDelta);
	void						Start_Attack_Move(_double dTimeDelta);
	void						Start_Attack_Move_1(_double dTimeDelta);
	void						Start_Attack_Move_2(_double dTimeDelta);
	void						Start_Attack_Splash(_double dTimeDelta);
	void						Start_Attack_TailWhip(_double dTimeDelta);
	void						Start_Attack_Water_Ball(_double dTimeDelta);
	void						Start_Attack_Spin(_double dTimeDelta);
	void						Start_Attack_Water_Beam_Start(_double dTimeDelta);
	void						Start_Attack_Water_Beam_Right_Loop(_double dTimeDelta);
	void						Start_Attack_Water_Beam_Right_End(_double dTimeDelta);
	void						Start_Attack_Water_Beam_Left_Loop(_double dTimeDelta);
	void						Start_Attack_Water_Beam_Left_End(_double dTimeDelta);
	void						Start_Attack_Water_Tornado(_double dTimeDelta);
	void						Start_Attack_Wing(_double dTimeDelta);
	void						Start_Fly_Move_1(_double dTimeDelta);
	void						Start_Fly_Move_2(_double dTimeDelta);
	void						Start_Turn_Right_180(_double dTimeDelta);
	void						Start_Turn_Right_90(_double dTimeDelta);
	void						Start_Turn_Left_180(_double dTimeDelta);
	void						Start_Turn_Left_90(_double dTimeDelta);
	void						Start_Groggy_Down(_double dTimeDelta);
	void						Start_Groggy_Up(_double dTimeDelta);
	void						Start_Damage_Down_Start(_double dTimeDelta);
	void						Start_Damage_Down_Loop(_double dTimeDelta);

private:
	void						Tick_Appear(_double dTimeDelta);
	void						Tick_Idle_Start(_double dTimeDelta);
	void						Tick_Idle_Loop(_double dTimeDelta);
	void						Tick_Idle_End(_double dTimeDelta);
	void						Tick_Warp_1(_double dTimeDelta);
	void						Tick_Warp_2(_double dTimeDelta);
	void						Tick_Attack_Bite(_double dTimeDelta);
	void						Tick_Attack_Lightning_Start(_double dTimeDelta);
	void						Tick_Attack_Lightning_Loop(_double dTimeDelta);
	void						Tick_Attack_Lightning_End(_double dTimeDelta);
	void						Tick_Attack_Move(_double dTimeDelta);
	void						Tick_Attack_Move_1(_double dTimeDelta);
	void						Tick_Attack_Move_2(_double dTimeDelta);
	void						Tick_Attack_Splash(_double dTimeDelta);
	void						Tick_Attack_TailWhip(_double dTimeDelta);
	void						Tick_Attack_Water_Ball(_double dTimeDelta);
	void						Tick_Attack_Spin(_double dTimeDelta);
	void						Tick_Attack_Water_Beam_Start(_double dTimeDelta);
	void						Tick_Attack_Water_Beam_Right_Loop(_double dTimeDelta);
	void						Tick_Attack_Water_Beam_Right_End(_double dTimeDelta);
	void						Tick_Attack_Water_Beam_Left_Loop(_double dTimeDelta);
	void						Tick_Attack_Water_Beam_Left_End(_double dTimeDelta);
	void						Tick_Attack_Water_Tornado(_double dTimeDelta);
	void						Tick_Attack_Wing(_double dTimeDelta);
	void						Tick_Fly_Move_1(_double dTimeDelta);
	void						Tick_Fly_Move_2(_double dTimeDelta);
	void						Tick_Turn_Right_180(_double dTimeDelta);
	void						Tick_Turn_Right_90(_double dTimeDelta);
	void						Tick_Turn_Left_180(_double dTimeDelta);
	void						Tick_Turn_Left_90(_double dTimeDelta);
	void						Tick_Groggy_Down(_double dTimeDelta);
	void						Tick_Groggy_Up(_double dTimeDelta);
	void						Tick_Damage_Down_Start(_double dTimeDelta);
	void						Tick_Damage_Down_Loop(_double dTimeDelta);

private:
	void						End_Appear(_double dTimeDelta);
	void						End_Idle_Start(_double dTimeDelta);
	void						End_Idle_Loop(_double dTimeDelta);
	void						End_Idle_End(_double dTimeDelta);
	void						End_Warp_1(_double dTimeDelta);
	void						End_Warp_2(_double dTimeDelta);
	void						End_Attack_Bite(_double dTimeDelta);
	void						End_Attack_Lightning_Start(_double dTimeDelta);
	void						End_Attack_Lightning_Loop(_double dTimeDelta);
	void						End_Attack_Lightning_End(_double dTimeDelta);
	void						End_Attack_Move(_double dTimeDelta);
	void						End_Attack_Move_1(_double dTimeDelta);
	void						End_Attack_Move_2(_double dTimeDelta);
	void						End_Attack_Splash(_double dTimeDelta);
	void						End_Attack_TailWhip(_double dTimeDelta);
	void						End_Attack_Water_Ball(_double dTimeDelta);
	void						End_Attack_Spin(_double dTimeDelta);
	void						End_Attack_Water_Beam_Start(_double dTimeDelta);
	void						End_Attack_Water_Beam_Right_Loop(_double dTimeDelta);
	void						End_Attack_Water_Beam_Right_End(_double dTimeDelta);
	void						End_Attack_Water_Beam_Left_Loop(_double dTimeDelta);
	void						End_Attack_Water_Beam_Left_End(_double dTimeDelta);
	void						End_Attack_Water_Tornado(_double dTimeDelta);
	void						End_Attack_Wing(_double dTimeDelta);
	void						End_Fly_Move_1(_double dTimeDelta);
	void						End_Fly_Move_2(_double dTimeDelta);
	void						End_Turn_Right_180(_double dTimeDelta);
	void						End_Turn_Right_90(_double dTimeDelta);
	void						End_Turn_Left_180(_double dTimeDelta);
	void						End_Turn_Left_90(_double dTimeDelta);
	void						End_Groggy_Down(_double dTimeDelta);
	void						End_Groggy_Up(_double dTimeDelta);
	void						End_Damage_Down_Start(_double dTimeDelta);
	void						End_Damage_Down_Loop(_double dTimeDelta);

private:
	_bool						Animation_Finish();

	_bool						Ready_Attack();				// ATT_SPLASH, ATT_SPIN
	_bool						Ready_Attack_Close();		// ATT_BITE, ATT_TAIL_WHIP, ATT_WATER_TORNADO, ATT_WING
	_bool						Ready_Attack_Far();		// ATT_MOVE, ATT_WATER_BALL, ATT_WATER_BEAM
	_bool						Ready_Attack_Bite();
	_bool						Ready_Attack_Lightning();
	_bool						Ready_Attack_Move();
	_bool						Ready_Attack_Splash();
	_bool						Ready_Attack_TailWhip();
	_bool						Ready_Attack_Water_Ball();
	_bool						Ready_Attack_Spin();
	_bool						Ready_Attack_Water_Beam();
	_bool						Ready_Attack_Water_Tornado();
	_bool						Ready_Attack_Wing();

	_bool						To_Target_Angle_R180();
	_bool						To_Target_Angle_L180();
	_bool						To_Target_Angle_R90();
	_bool						To_Target_Angle_L90();
	_bool						Target_On_Right();
	_bool						Target_On_Left();
	_bool						PosY_Less_Neg50();
	_bool						Check_IdleFinishCount();
	_bool						Check_LightningCount();
	_bool						Check_HP_Groggy();
	_bool						Check_HP_Die();

public:
	static CLeviathan_State*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, class CLeviathan* pLeviathan, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	virtual void					Free() override;
};

END