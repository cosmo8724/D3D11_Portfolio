#include "..\Public\LevelMgr.h"
#include "Level.h"

IMPLEMENT_SINGLETON(CLevelMgr)

CLevelMgr::CLevelMgr()
{
}

HRESULT CLevelMgr::Open_Level(CLevel * pNewLevel)
{
	NULL_CHECK_RETURN(pNewLevel, E_FAIL);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	return S_OK;
}

void CLevelMgr::Tick(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pCurrentLevel, );

	m_pCurrentLevel->Tick(dTimeDelta);
}

void CLevelMgr::Late_Tick(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pCurrentLevel, );

	m_pCurrentLevel->Late_Tick(dTimeDelta);
}

HRESULT CLevelMgr::Render()
{
	NULL_CHECK_RETURN(m_pCurrentLevel, E_FAIL);

	return m_pCurrentLevel->Render();
}

void CLevelMgr::Free()
{
	Safe_Release(m_pCurrentLevel);
}
