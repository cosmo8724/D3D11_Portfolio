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

class CBaine_State final : public CBase
{
public:
	enum ANIMATION {
		IDLE_HAPPY_LOOP, WALK_LOOP, SURPRISED, FREED_FROM_GLOOM, IDLE_STILL_LOOP, IDLE_BASE_LOOP, TALK_BASE_LOOP
	};

private:
	CBaine_State();
	virtual ~CBaine_State() = default;

public:
	HRESULT				Initialize(class CBaine* pBaine, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	void					Tick(_double dTimeDelta);
	void					Late_Tick(_double dTImeDelta);

private:
	HRESULT				SetUp_State_Idle();

private:
	CGameInstance*		m_pGameInstance = nullptr;

private:
	class CBaine*			m_pBaine = nullptr;
	CStateMachine*		m_pStateMachine = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	void					Start_Idle_Base_Loop(_double dTimeDelta);
	void					Start_Idle_Happy_Loop(_double dTimeDelta);
	void					Start_Idle_Still_Loop(_double dTimeDelta);

private:
	void					Tick_Idle_Base_Loop(_double dTimeDelta);
	void					Tick_Idle_Happy_Loop(_double dTimeDelta);
	void					Tick_Idle_Still_Loop(_double dTimeDelta);

private:
	void					End_Idle_Base_Loop(_double dTimeDelta);
	void					End_Idle_Happy_Loop(_double dTimeDelta);
	void					End_Idle_Still_Loop(_double dTimeDelta);

private:
	_bool					Animation_Finish();

public:
	static CBaine_State*	Create(class CBaine* pBaine, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	virtual void				Free() override;
};

END