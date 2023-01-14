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
	void				Set_TimeScale(_double& dTimeDelta, _double dTimeScale) {
		m_dTimeScale = dTimeScale;
		dTimeDelta = (_double)(m_CurrentTime.QuadPart - m_OldTimeQuadpart) / (_double)(m_CpuTick.QuadPart) * m_dTimeScale;
	}

public:
	HRESULT			Ready_Timer();
	void				Update_Timer();

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_double				m_dTimeDelta = 0.0;
	_double				m_dTimeScale = 1.0;
	LONGLONG			m_OldTimeQuadpart = 0;

public:
	static CTimer*	Create();
	virtual void		Free() override;
};

END