#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

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

	FAILED_CHECK_RETURN(m_pGameInstance->Initialize_Engine(LEVEL_END, tGraphicDesc, &m_pGraphicDev, &m_pDeviceContext), E_FAIL);
	
	FAILED_CHECK_RETURN(Ready_Prototype_Component(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype_GameObject(), E_FAIL);

	FAILED_CHECK_RETURN(Start_Level(LEVEL_LOGO), E_FAIL);

	pTool = CTestTool::Create();

	return S_OK;
}

void CMainApp::Tick(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pGameInstance, );

	m_pGameInstance->Tick_Engine(dTimeDelta);
}

HRESULT CMainApp::Render()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	m_pGameInstance->ImGui_Render();
	m_pGameInstance->Clear_Graphic_Device(&_float4(0.f, 0.f, 0.8f, 1.f));
	m_pGameInstance->Render_Level();
	m_pGameInstance->ImGui_Render_Update();
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Resize_BackBuffer()
{
	GRAPHIC_DESC	tGraphicDesc;
	ZeroMemory(&tGraphicDesc, sizeof(GRAPHIC_DESC));

	tGraphicDesc.hWnd = g_hWnd;
	if (!g_bFullScreen)
	{
		tGraphicDesc.iViewportSizeX = g_iWinSizeX;
		tGraphicDesc.iViewportSizeY = g_iWinSizeY;
	}
	else
	{
		tGraphicDesc.iViewportSizeX = GetSystemMetrics(SM_CXSCREEN);
		tGraphicDesc.iViewportSizeY = GetSystemMetrics(SM_CYSCREEN);
	}

	FAILED_CHECK_RETURN(m_pGameInstance->Update_SwapChain(tGraphicDesc.hWnd, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, g_bFullScreen, g_bNeedResizeSwapChain), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevel)
{
	if (LEVEL_LOADING == eLevel || nullptr == m_pGameInstance)
		return E_FAIL;
	
	FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphicDev, m_pDeviceContext, eLevel)), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(LEVEL_PUBLIC, L"Prototype_Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);
	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
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
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGraphicDev);

	CGameInstance::Release_Engine();
}
