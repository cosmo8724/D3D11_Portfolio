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

class CKlemens_Klout_State final : public CBase
{
public:
	enum ANIMATION {
		ª‰COWERING_LOOP, DAMAGED, IDLE_ANGRY_LOOP, IDLE_HAPPY_LOOP, IDLE_BASE_LOOP, IDLE_SAD_LOOP, SURPRISED, TALK_BASE_LOOP, WALK_LOOP
	};

private:
	CKlemens_Klout_State();
	virtual ~CKlemens_Klout_State() = default;

public:
	HRESULT				Initialize(class CKlemens_Klout* pKlout, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	void					Tick(_double dTimeDelta);
	void					Late_Tick(_double dTImeDelta);

private:
	HRESULT				SetUp_State_Idle();
	HRESULT				SetUp_State_Surprised();
	HRESULT				SetUp_State_Talk();

private:
	CGameInstance*		m_pGameInstance = nullptr;

private:
	class CKlemens_Klout*	m_pKlout = nullptr;
	CStateMachine*		m_pStateMachine = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	_bool					m_bSurprised = false;

private:
	void					Start_Idle_Happy_Loop(_double dTimeDelta);
	void					Start_Surprised(_double dTimeDelta);
	void					Start_Talk(_double dTimeDelta);

private:
	void					Tick_Idle_Happy_Loop(_double dTimeDelta);
	void					Tick_Surprised(_double dTimeDelta);
	void					Tick_Talk(_double dTimeDelta);

private:
	void					End_Idle_Happy_Loop(_double dTimeDelta);
	void					End_Surprised(_double dTimeDelta);
	void					End_Talk(_double dTimeDelta);

private:
	_bool					Animation_Finish();
	_bool					Surprised();
	_bool					Talking();
	_bool					NotTalking();

public:
	static CKlemens_Klout_State*	Create(class CKlemens_Klout* pKlout, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	virtual void				Free() override;
};

END