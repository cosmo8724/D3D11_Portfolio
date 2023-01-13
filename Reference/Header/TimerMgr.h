#pragma once
#include "Base.h"

BEGIN(Engine)

class CTimerMgr : public CBase
{
	DECLARE_SINGLETON(CTimerMgr)

private:
	CTimerMgr();
	virtual ~CTimerMgr() = default;

public:
	const _double		Get_TimeDelta(const wstring wstrTimerTag);
	void				Set_TimeScale(const wstring& wstrTimerTag, _double dTimeScale);

public:
	HRESULT			Ready_Timer(const wstring wstrTimerTag);
	void				Update_Timer(const wstring wstrTimerTag);

private:
	class CTimer*	Find_Timer(const wstring wstrTimerTag);

private:
	map<const wstring, class CTimer*>				m_mapTimer;
	typedef map<const wstring, class CTimer*>		TIMERS;

public:
	virtual void		Free() override;
};

END