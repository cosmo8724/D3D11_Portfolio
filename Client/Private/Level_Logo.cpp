#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Tool_Property.h"
#include "Tool_Settings.h"
#include "Tool_Debugger.h"
#include "Tool_LevelViewer.h"
#include "TestTool.h"
#include "Tool_MapEditor.h"
#include "Tool_AnimationManager.h"
#include "Tool_PrototypeMgr.h"
#include "Tool_ModelSave.h"

CLevel_Logo::CLevel_Logo(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_BackGround(L"Layer_BackGround"), E_FAIL);

	CGameInstance::GetInstance()->Clear_ImGuiObject();
	//CGameInstance::GetInstance()->Add_ImGuiTabObject(CTool_Property::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_Settings::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_Debugger::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_LevelViewer::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_PrototypeMgr::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_MapEditor::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_AnimationManager::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTestTool::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_ModelSave::Create(m_pDevice, m_pContext));

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
