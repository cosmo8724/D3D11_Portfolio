#include "stdafx.h"
#include "..\Public\Level_TestStage.h"

CLevel_TestStage::CLevel_TestStage(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_TestStage::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	return S_OK;
}

void CLevel_TestStage::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CLevel_TestStage::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CLevel_TestStage::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	SetWindowText(g_hWnd, L"Level : Test Stage");

	return S_OK;
}

CLevel_TestStage * CLevel_TestStage::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CLevel_TestStage*		pInstance = new CLevel_TestStage(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_TestStage::Free()
{
	__super::Free();
}
