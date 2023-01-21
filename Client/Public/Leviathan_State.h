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
		APPEAR,
		ATT_BIG_WAVE, ATT_BITE,
		ATT_ENERGY_CHARGE_ST, ATT_ENERGY_CHARGE_LP, ATT_ENERGY_CHARGE_ED,
		ATT_MOVE_1, ATT_MOVE_2, ATT_MOVE_3, ATT_SPIN, ATT_SPLASH, ATT_TAIL_WHIP, ATT_WATER_BALL,
		ATT_WATER_BEAM_ST, ATT_WATER_BEAM_RIGHT_LP, ATT_WATER_BEAM_RIGHT_ED, ATT_WATER_BEAM_LEFT_LP, ATT_WATER_BEAM_LEFT_ED,
		ATT_WATER_POLE, ATT_WATER_TORNADO, ATT_WATER_WING,
		DMG_BACK, DMG_DOWN_ST, DMG_DOWN_LP, DMG_DOWN_ED,
		DIE_1, DIE_2, DIE_3,
		DMG_FRONT, DMG_LEFT, DMG_RIGHT, DIE_DOWN,
		FLY_MOVE_1, FLY_MOVE_2,
		IDLE_ST, IDLE_LP, IDLE_ED,
		TURN_RIGHT_180, TURN_RIGHT_90, TURN_LEFT_180, TURN_LEFT_90,
		WARP_1, WARP_2
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

private:

private:

private:
	_bool						Animation_Finish();

public:
	static CLeviathan_State*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, class CLeviathan* pLeviathan, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	virtual void					Free() override;
};

END