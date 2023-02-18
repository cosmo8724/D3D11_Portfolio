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

class CGrandma_State final : public CBase
{
public:
	enum ANIMATION {
		COWERING_LOOP, DAMAGED, IDLE_ANGRY_LOOP, IDLE_BASE_LOOP, IDLE_HAPPY_LOOP, IDLE_SAD_LOOP, IDLE_STILL_LOOP, IDLE_TINKER_GROUND_LOOP, IDLE_TINKER_WALL_LOOP,
		SURPRISED, TALK_BASE_LOOP, WALK_LOOP
	};
	
public:
	CGrandma_State();
	virtual ~CGrandma_State() = default;

public:
	HRESULT				Initialize(class CGrandma* pGrandma, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	void					Tick(_double dTimeDelta);
	void					Late_Tick(_double dTImeDelta);

private:
	HRESULT				SetUp_State_Idle();
	HRESULT				SetUp_State_Surprised();
	HRESULT				SetUp_State_Talk();

private:
	CGameInstance*		m_pGameInstance = nullptr;

private:
	class CGrandma*		m_pGrandma = nullptr;
	CStateMachine*		m_pStateMachine = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	_bool					m_bSurprised = false;

private:
	void					Start_Idle_Still_Loop(_double dTimeDelta);
	void					Start_Surprised(_double dTimeDelta);
	void					Start_Talk(_double dTimeDelta);

private:
	void					Tick_Idle_Still_Loop(_double dTimeDelta);
	void					Tick_Surprised(_double dTimeDelta);
	void					Tick_Talk(_double dTimeDelta);

private:
	void					End_Idle_Still_Loop(_double dTimeDelta);
	void					End_Surprised(_double dTimeDelta);
	void					End_Talk(_double dTimeDelta);

private:
	_bool					Animation_Finish();
	_bool					Surprised();
	_bool					Talking();
	_bool					NotTalking();

public:
	static CGrandma_State*	Create(class CGrandma* pGrandma, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	virtual void				Free() override;
};

END