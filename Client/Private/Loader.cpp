#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"


CLoader::CLoader(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint	APIENTRY		LoadingThread(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevel())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;

	case LEVEL_TESTSTAGE:
		pLoader->Loading_ForGamePlay();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_wstrLoadingText = L"Loading Textures...";
	m_wstrLoadingText = L"Loading Buffer...";
	m_wstrLoadingText = L"Loading Models...";
	m_wstrLoadingText = L"Loading Shader...";
	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_BackGround", CBackGround::Create(m_pDevice, m_pContext)), E_FAIL);
	m_wstrLoadingText = L"Complete Loading!";

	m_bIsFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_wstrLoadingText = L"Loading Textures...";
	m_wstrLoadingText = L"Loading Buffer...";
	m_wstrLoadingText = L"Loading Models...";
	m_wstrLoadingText = L"Loading Shader...";
	m_wstrLoadingText = L"Create Prototype Objects...";
	m_wstrLoadingText = L"Complete Loading!";

	m_bIsFinished = true;

	Safe_Release(pGameInstance);
	return S_OK;
}

CLoader * CLoader::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, LEVEL eNextLevel)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}