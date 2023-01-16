#include "stdafx.h"
#include "..\Public\SigridCutScn_State.h"
#include "GameInstance.h"
#include "Sigrid_CutScn.h"
#include "GameUtility.h"

CSigridCutScn_State::CSigridCutScn_State()
{
}

HRESULT CSigridCutScn_State::Initialize(CSigrid_CutScn * pPlayer, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	//m_pGameInstance = CGameInstance::GetInstance();

	m_pPlayer = pPlayer;
	m_pStateMachine = pStateMachineCom;
	m_pModelCom = pModel;
	m_pTransformCom = pTransform;

	return S_OK;
}

void CSigridCutScn_State::Tick(_double & dTimeDelta)
{
}

void CSigridCutScn_State::Late_Tick(_double & dTimeDelta)
{
}

_bool CSigridCutScn_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

CSigridCutScn_State * CSigridCutScn_State::Create(CSigrid_CutScn * pPlayer, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	CSigridCutScn_State*	pInstance = new CSigridCutScn_State;

	if (FAILED(pInstance->Initialize(pPlayer, pStateMachineCom, pModel, pTransform)))
	{
		MSG_BOX("Failed to Create : CSigridCutScn_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSigridCutScn_State::Free()
{
}