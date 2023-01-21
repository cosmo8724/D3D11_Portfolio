#include "stdafx.h"
#include "..\Public\Leviathan_State.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Leviathan.h"
#include "Sigrid.h"

CLeviathan_State::CLeviathan_State()
{
}

HRESULT CLeviathan_State::Initialize(DEVICE pDevice, DEVICE_CONTEXT pContext, CLeviathan * pLeviathan, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = CGameInstance::GetInstance();

	m_pLeviathan = pLeviathan;
	m_pStateMachineCom = pStateMachineCom;
	m_pModelCom = pModel;
	m_pTransformCom = pTransform;

	FAILED_CHECK_RETURN(SetUp_State_Idle(), E_FAIL);

	return S_OK;
}

void CLeviathan_State::Tick(_double dTimeDelta)
{
}

void CLeviathan_State::Late_Tick(_double dTimeDelta)
{
}

HRESULT CLeviathan_State::SetUp_State_Idle()
{
	return S_OK;
}

_bool CLeviathan_State::Animation_Finish()
{
	if (m_pModelCom->Get_AnimationFinish() == true)
		return true;

	return false;
}

CLeviathan_State * CLeviathan_State::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CLeviathan * pLeviathan, CStateMachine * pStateMachineCom, CModel * pModel, CTransform * pTransform)
{
	CLeviathan_State*	pInstance = new CLeviathan_State;

	if (FAILED(pInstance->Initialize(pDevice, pContext, pLeviathan, pStateMachineCom, pModel, pTransform)))
	{
		MSG_BOX("Failed to Create : CLeviathan_State");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLeviathan_State::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}