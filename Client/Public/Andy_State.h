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

class CAndy_State final : public CBase
{
public:
	enum ANIMATION {
		COWERING_LOOP, WALK_LOOP, IDLE_BASE_LOOP, TALK_BASE_LOOP, FREEDOM_FROM_GLOOM, SURPRISED
	};

private:
	CAndy_State();
	virtual ~CAndy_State() = default;

public:
	HRESULT				Initialize(class CAndy* pAndy, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	void					Tick(_double dTimeDelta);
	void					Late_Tick(_double dTImeDelta);

private:
	HRESULT				SetUp_State_Idle();
	HRESULT				SetUp_State_Surprised();
	HRESULT				SetUp_State_Talk();

private:
	CGameInstance*		m_pGameInstance = nullptr;

private:
	class CAndy*			m_pAndy = nullptr;
	CStateMachine*		m_pStateMachine = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

private:
	_bool					m_bSurprised = false;

private:
	void					Start_Cowering(_double dTimeDelta);
	void					Start_Surprised(_double dTimeDelta);
	void					Start_Talk(_double dTimeDelta);

private:
	void					Tick_Cowering(_double dTimeDelta);
	void					Tick_Surprised(_double dTimeDelta);
	void					Tick_Talk(_double dTimeDelta);

private:
	void					End_Cowering(_double dTimeDelta);
	void					End_Surprised(_double dTimeDelta);
	void					End_Talk(_double dTimeDelta);

private:
	_bool					Animation_Finish();
	_bool					Surprised();
	_bool					Talking();
	_bool					NotTalking();

public:
	static CAndy_State*		Create(class CAndy* pAndy, CStateMachine* pStateMachineCom, CModel* pModelCom, CTransform* pTransformCom);
	virtual void				Free() override;
};

END