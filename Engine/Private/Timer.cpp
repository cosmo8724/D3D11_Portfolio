#include "stdafx.h"
#include "..\Public\Timer.h"

CTimer::CTimer()
{
	ZeroMemory(&m_CurrentTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OriginTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Ready_Timer()
{
	QueryPerformanceCounter(&m_CurrentTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_OriginTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void CTimer::Update_Timer()
{
	QueryPerformanceCounter(&m_CurrentTime);

	if (m_CurrentTime.QuadPart - m_OriginTime.QuadPart > m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_OriginTime = m_CurrentTime;
	}

	m_dTimeDelta = (_double)(m_CurrentTime.QuadPart - m_OldTime.QuadPart) / (_double)(m_CpuTick.QuadPart) * m_dTimeScale;
	m_OldTimeQuadpart = m_OldTime.QuadPart;

	m_OldTime = m_CurrentTime;
}

CTimer * CTimer::Create()
{
	CTimer*	pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
	{
		MSG_BOX("Failed to Create : CTimer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}