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

class CCritter_Small_State final : public CBase
{
public:
	enum ANIMATION {
		DEATH, EXPLOSION_SHAKE, IDLE_STILL_LOOP, IDLE_STUN_LOOP, SPRINT_LOOP, STAGGER, STUN_END, TACKLE, WALK_LOOP
	};

private:
	CCritter_Small_State();
	virtual ~CCritter_Small_State() = default;

public:
	HRESULT					Initialize(class CCritter_Small* pCritter, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	void						Tick(_double dTimeDelta);
	void						Late_Tick(_double dTimeDelta);

private:
	HRESULT					SetUp_State_Idle();
	HRESULT					SetUp_State_Stun();
	HRESULT					SetUp_State_Walk();
	HRESULT					SetUp_State_Sprint();
	HRESULT					SetUp_State_Tackle();
	HRESULT					SetUp_State_Stagger();
	HRESULT					SetUp_State_Death();
	HRESULT					SetUp_State_Explosion();

private:
	class CGameInstance*	m_pGameInstance = nullptr;

private:
	class CCritter_Small*	m_pCritter = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:
	_uint						m_iIdleFinishCount = 0;
	_uint						m_iStunFinishCount = 0;
	_double					m_dWalkTime = 0.0;

private:
	void						Start_Idle(_double dTimeDelta);
	void						Start_Stun(_double dTimeDelta);
	void						Start_Stun_End(_double dTimeDelta);
	void						Start_Walk(_double dTimeDelta);
	void						Start_Sprint(_double dTimeDelta);
	void						Start_Tackle(_double dTimeDelta);
	void						Start_Stagger(_double dTimeDelta);
	void						Start_Death(_double dTimeDelta);

private:
	void						Tick_Idle(_double dTimeDelta);
	void						Tick_Stun(_double dTimeDelta);
	void						Tick_Stun_End(_double dTimeDelta);
	void						Tick_Walk(_double dTimeDelta);
	void						Tick_Sprint(_double dTimeDelta);
	void						Tick_Tackle(_double dTimeDelta);
	void						Tick_Stagger(_double dTimeDelta);
	void						Tick_Death(_double dTimeDelta);

private:
	void						End_Idle(_double dTimeDelta);
	void						End_Stun(_double dTImeDelta);
	void						End_Stun_End(_double dTimeDelta);
	void						End_Walk(_double dTimeDelta);
	void						End_Sprint(_double dTimeDelta);
	void						End_Tackle(_double dTimeDelta);
	void						End_Stagger(_double dTimeDelta);
	void						End_Death(_double dTimeDelta);

private:
	_bool						KeyDown_F5();

	_bool						Animation_Finish();
	_bool						Ready_Attack();
	_bool						Damaged();
	_bool						Damaged_Special();
	_bool						Dead();

	_bool						Player_Detected();
	_bool						Player_DetectedAndFar();
	_bool						Player_DetectedAndClose();
	_bool						Player_NotDetected();

	_bool						Check_IdleFinishCount();
	_bool						Check_StunFinishCount();
	_bool						Check_WalkTime();

public:
	static CCritter_Small_State*	Create(class CCritter_Small* pCritter, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	virtual void						Free() override;
};

END