#include "stdafx.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_TestStage.h"

CLevel_Loading::CLevel_Loading(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	NULL_CHECK_RETURN(m_pLoader, E_FAIL);

	return S_OK;
}

void CLevel_Loading::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CLevel_Loading::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	CGameInstance*	pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == m_pLoader->IsFinished())
	{
		if (pGameInstance->Mouse_Down(DIM_LB))
		{
			CLevel*	pLevel = nullptr;

			switch (m_eNextLevel)
			{
			case LEVEL_LOGO:
				pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
				break;

			case LEVEL_TESTSTAGE:
				pLevel = CLevel_TestStage::Create(m_pDevice, m_pContext);
				break;
			}

			if (FAILED(pLevel == nullptr || pGameInstance->Open_Level(m_eNextLevel, pLevel)))
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
	}
	Safe_Release(pGameInstance);
}

HRESULT CLevel_Loading::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText().c_str());

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, LEVEL eNextLevel)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
