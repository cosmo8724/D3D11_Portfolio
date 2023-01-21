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
		ATT_BITE, PHASE_CHANGE_READY_ST, PHASE_CHANGE_READY_LP, PHASE_CHANGE_READY_ED,
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

	enum PHASE { PHASE_1, PHASE_2, PHASE_END };

	enum PHASE_1_PATTERN {
		BITE, MOVE, SPLASH, TAIL_WHIP, WATER_BALL, PHASE_1_PATTERN_END
	};

	enum PHASE_2_PATTERN {
		SPIN = 5, WATER_BEAM, WATER_TORNADO, WING, PHASE_2_PATTERN_END
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
	HRESULT					SetUp_State_Attack();
	HRESULT					SetUp_State_Attack_Energy_Charge();
	HRESULT					SetUp_State_Attack_Move();
	HRESULT					SetUp_State_Attack_Water_Beam();
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
	PHASE						m_ePhase = PHASE_1;
	_uint						m_iIdleFinishCount = 0;

private:
	void						Start_Appear(_double dTimeDelta);
	void						Start_Idle_Start(_double dTimeDelta);
	void						Start_Idle_Loop(_double dTimeDelta);
	void						Start_Idle_End(_double dTimeDelta);
	void						Start_Warp_1(_double dTimeDelta);
	void						Start_Warp_2(_double dTimeDelta);

private:
	void						Tick_Appear(_double dTimeDelta);
	void						Tick_Idle_Start(_double dTimeDelta);
	void						Tick_Idle_Loop(_double dTimeDelta);
	void						Tick_Idle_End(_double dTimeDelta);
	void						Tick_Warp_1(_double dTimeDelta);
	void						Tick_Warp_2(_double dTimeDelta);

private:
	void						End_Appear(_double dTimeDelta);
	void						End_Idle_Start(_double dTimeDelta);
	void						End_Idle_Loop(_double dTimeDelta);
	void						End_Idle_End(_double dTimeDelta);
	void						End_Warp_1(_double dTimeDelta);
	void						End_Warp_2(_double dTimeDelta);

private:
	_bool						Animation_Finish();

	_bool						Check_IdleFinishCount();

public:
	static CLeviathan_State*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, class CLeviathan* pLeviathan, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	virtual void					Free() override;
};

END