#include "stdafx.h"
#include "..\Public\StateMachine.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

CStateMachine::CStateMachine(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine & rhs)
	: CComponent(rhs)
{
}

CStateMachine & CStateMachine::Set_Root(const wstring & wstrStateName)
{
	m_wstrRootStateName = wstrStateName;

	return *this;
}

CStateMachine & CStateMachine::Add_State(const wstring & wstrStateName)
{
	STATE		tState;
	ZeroMemory(&tState, sizeof(STATE));

	m_mapState.emplace(wstrStateName, tState);
	
	m_wstrCurrentStateName = wstrStateName;

	return *this;
}

HRESULT CStateMachine::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CStateMachine::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

void CStateMachine::Tick(_double & dTimeDelta)
{
	auto	CurState = find_if(m_mapState.begin(), m_mapState.end(), CTag_Finder(m_wstrCurrentStateName));

	for (auto& Changer : m_mapChanger[m_wstrCurrentStateName])
	{
		if (Changer.Changer_Func() == true)
		{
			m_wstrNextStateName = Changer.wstrNextState;
			break;
		}
	}

	if (m_wstrNextStateName != L"")
	{
		if (CurState->second.State_End != nullptr)
			CurState->second.State_End(dTimeDelta);

		m_wstrLastStateName = m_wstrCurrentStateName;
		m_wstrCurrentStateName = m_wstrNextStateName;

		if (m_mapState[m_wstrCurrentStateName].State_Start != nullptr)
			m_mapState[m_wstrCurrentStateName].State_Start(dTimeDelta);

		m_wstrNextStateName = L"";
	}
	else
	{
		if (CurState->second.State_Tick != nullptr)
			CurState->second.State_Tick(dTimeDelta);
	}
}

CStateMachine * CStateMachine::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CStateMachine*		pInstance = new CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CStateMachine::Clone(CGameObject * pOwner, void * pArg)
{
	CStateMachine*		pInstance = new CStateMachine(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();
}
