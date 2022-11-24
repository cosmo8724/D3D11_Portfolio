#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	const _double	&	Get_TimeDelta() const { return m_dTimeDelta; }

public:
	HRESULT			Ready_Timer();
	void				Update_Timer();

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_double				m_dTimeDelta = 0.0;

public:
	static CTimer*	Create();
	virtual void		Free() override;
};

END