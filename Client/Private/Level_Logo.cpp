#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CLevel_Logo::CLevel_Logo(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	return S_OK;
}

void CLevel_Logo::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CLevel_Logo::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (pGameInstance->Key_Down(DIK_RETURN))
		FAILED_CHECK_RETURN(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TESTSTAGE)), );

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Logo::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	SetWindowText(g_hWnd, L"Level : Logo");

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_BackGround"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
