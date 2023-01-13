#include "stdafx.h"
#include "..\Public\TimerMgr.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimerMgr)

CTimerMgr::CTimerMgr()
{
}

const _double CTimerMgr::Get_TimeDelta(const wstring wstrTimerTag)
{
	CTimer*	pTimer = Find_Timer(wstrTimerTag);

	NULL_CHECK_RETURN(pTimer, 0.0);

	return pTimer->Get_TimeDelta();
}

void CTimerMgr::Set_TimeScale(const wstring & wstrTimerTag, _double dTimeScale)
{
	CTimer*	pTimer = Find_Timer(wstrTimerTag);
	NULL_CHECK_RETURN(pTimer, );

	pTimer->Set_TimeScale(dTimeScale);
}

HRESULT CTimerMgr::Ready_Timer(const wstring wstrTimerTag)
{
	CTimer*	pTimer = Find_Timer(wstrTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	
	NULL_CHECK_RETURN(pTimer, E_FAIL);

	m_mapTimer.insert({ wstrTimerTag, pTimer });

	return S_OK;
}

void CTimerMgr::Update_Timer(const wstring wstrTimerTag)
{
	CTimer*	pTimer = Find_Timer(wstrTimerTag);

	NULL_CHECK_RETURN(pTimer, );

	pTimer->Update_Timer();
}

CTimer * CTimerMgr::Find_Timer(const wstring wstrTimerTag)
{
	auto	iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(wstrTimerTag));

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void CTimerMgr::Free()
{
	for (auto& Pair : m_mapTimer)
		Safe_Release(Pair.second);

	m_mapTimer.clear();
}
