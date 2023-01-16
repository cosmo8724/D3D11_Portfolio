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

class CSigridCutScn_State :
	public CBase
{
public:
	CSigridCutScn_State();
	virtual ~CSigridCutScn_State() = default;

public:
	HRESULT					Initialize(class CSigrid_CutScn* pPlayer, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	void						Tick(_double& dTimeDelta);
	void						Late_Tick(_double& dTimeDelta);

private:
	class CGameInstance*	m_pGameInstance = nullptr;

private:
	class CSigrid_CutScn*	m_pPlayer = nullptr;
	CStateMachine*			m_pStateMachine = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

private:

private:

private:

private:
	_bool						Animation_Finish();

public:
	static CSigridCutScn_State*	Create(class CSigrid_CutScn* pPlayer, CStateMachine* pStateMachineCom, CModel* pModel, CTransform* pTransform);
	virtual void						Free() override;
};

END