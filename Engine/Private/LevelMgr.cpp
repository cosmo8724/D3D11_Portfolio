#include "stdafx.h"
#include "..\Public\LevelMgr.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevelMgr)

CLevelMgr::CLevelMgr()
{
}

HRESULT CLevelMgr::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	NULL_CHECK_RETURN(pNewLevel, E_FAIL);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pCurrentLevel)
		pGameInstance->Clear_Level(m_iLevelIndex);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;
	m_iLevelIndex = iLevelIndex;

	Safe_Release(pGameInstance);

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
