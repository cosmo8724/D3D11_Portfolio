#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	GRAPHIC_DESC	tGraphicDesc;
	ZeroMemory(&tGraphicDesc, sizeof(GRAPHIC_DESC));
	
	tGraphicDesc.hInst = g_hInst;
	tGraphicDesc.hWnd = g_hWnd;
	tGraphicDesc.iViewportSizeX = g_iWinSizeX;
	tGraphicDesc.iViewportSizeY = g_iWinSizeY;
	tGraphicDesc.eWindowMode = GRAPHIC_DESC::WINMODE_WIN;

	FAILED_CHECK_RETURN(m_pGameInstance->Initialize_Engine(tGraphicDesc, &m_pGraphicDev, &m_pDeviceContext), E_FAIL);
	
	return S_OK;
}

void CMainApp::Tick(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pGameInstance, );

	m_pGameInstance->Tick_Engine(dTimeDelta);
	m_pGameInstance->ImGui_NewFrame(dTimeDelta);
}

HRESULT CMainApp::Render()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	m_pGameInstance->ImGui_EndFrame();
	m_pGameInstance->Clear_Graphic_Device(&_float4(0.3f, 0.3f, 0.3f, 1.f));
	m_pGameInstance->ImGui_Render();
	m_pGameInstance->Present();
	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("MainApp Create Failed");
		Safe_Release<CMainApp*>(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
