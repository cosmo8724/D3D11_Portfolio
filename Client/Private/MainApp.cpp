#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Dynamic_Camera.h"
#include "Static_Camera.h"
#include "Ocean.h"
#include "StateMachine.h"
#include "Cursor.h"
#include "Scene_Change.h"
#include "Sigrid.h"

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
	
	FAILED_CHECK_RETURN(Ready_Font(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype_Component(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Prototype_GameObject(), E_FAIL);

	FAILED_CHECK_RETURN(Start_Level(LEVEL_LOGO), E_FAIL);

	g_pCursor = dynamic_cast<CCursor*>(m_pGameInstance->Clone_GameObjectReturnPtr(LEVEL_END, L"Layer_Cursor", L"Prototype_GameObject_Cursor"));

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
	NULL_CHECK_RETURN(m_pRenderer, E_FAIL);

	m_pGameInstance->ImGui_Render();
	m_pGameInstance->Clear_Graphic_Device(&_float4(0.f, 0.f, 0.8f, 1.f));
	m_pRenderer->Draw_RenderGroup(g_bShopOpen);

	if (g_bShopOpen == true || g_bReadySceneChange == true)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, dynamic_cast<CSigrid*>(m_pGameInstance->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back()));
		m_pRenderer->Draw_RenderGroup(false);
	}

	m_pGameInstance->ImGui_Render_Update();
	m_pGameInstance->Render_Level();
	
	return m_pGameInstance->Present();

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
	
	FAILED_CHECK_RETURN(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphicDev, m_pDeviceContext, eLevel, L"../Bin/Save Data/Level_Logo_Prototype_Components_230116.json", L"../Bin/Save Data/Level_Logo_Prototype_GameObjects_230116.json", L"../Bin/Save Data/Level_Logo_CloneObjects_230116.json")), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Font()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Font(m_pGraphicDev, m_pDeviceContext, L"Font_DoongSil", L"../Bin/Resource/Font/DoongSil.spritefont"), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", m_pRenderer = CRenderer::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);
	Safe_AddRef(m_pRenderer);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", CShader::Create(m_pGraphicDev, m_pDeviceContext, L"../Bin/Shader/Shader_VtxTex.hlsl", CShader::DECLARATION_VTXTEX, VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", CVIBuffer_Rect::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", CStateMachine::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_Cursor", CTexture::Create(m_pGraphicDev, m_pDeviceContext, L"../Bin/Resource/Texture/UI/Cursor/CursorIcon_%d.png", 2)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_SceneChange", CTexture::Create(m_pGraphicDev, m_pDeviceContext, L"../Bin/Resource/Texture/UI/Scene Change FX/SS_BloodWipe_%d.png", 4)), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera_Dynamic", CDynamic_Camera::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Camera_Static", CStatic_Camera::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Ocean", COcean::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Cursor", CCursor::Create(m_pGraphicDev, m_pDeviceContext)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SceneChange_0", CScene_Change::Create(m_pGraphicDev, m_pDeviceContext, 0)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SceneChange_1", CScene_Change::Create(m_pGraphicDev, m_pDeviceContext, 1)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SceneChange_2", CScene_Change::Create(m_pGraphicDev, m_pDeviceContext, 2)), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SceneChange_3", CScene_Change::Create(m_pGraphicDev, m_pDeviceContext, 3)), E_FAIL);

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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGraphicDev);

	CGameInstance::Release_Engine();
}
