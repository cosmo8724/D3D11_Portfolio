#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
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
#include "Camera.h"
#include "Tool_Navigation.h"

CLevel_Logo::CLevel_Logo(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize(const wstring & wstrCloneObjFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Light(), E_FAIL);

	if (wstrCloneObjFilePath == L"")
	{
		FAILED_CHECK_RETURN(Ready_Layer_Camera(L"Layer_Camera"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_UI(L"Layer_UI"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_SkyBox(L"Layer_SkyBox"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_Ocean(L"Layer_Ocean"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_Player(L"Layer_Player"), E_FAIL);
		FAILED_CHECK_RETURN(Ready_Layer_Islands(L"Layer_Islands"), E_FAIL);
	}
	else
		FAILED_CHECK_RETURN(Load_CloneObjects(wstrCloneObjFilePath), E_FAIL);

	CGameInstance::GetInstance()->Clear_ImGuiObject();
	//CGameInstance::GetInstance()->Add_ImGuiTabObject(CTool_Property::Create());
	//CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTestTool::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_Settings::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_Debugger::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_LevelViewer::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_PrototypeMgr::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_MapEditor::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_AnimationManager::Create());
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_ModelSave::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImGuiWindowObject(CTool_Navigation::Create(m_pDevice, m_pContext));

	return S_OK;
}

void CLevel_Logo::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	CGameInstance::GetInstance()->Play_Sound(L"SFX_Menu_Music.wav", 1.f, true);
}

void CLevel_Logo::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (pGameInstance->Key_Down(DIK_RETURN))
		FAILED_CHECK_RETURN(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TESTSTAGE, L"../Bin/Save Data/Level_Stage_Prototype_Components_230127.json", L"../Bin/Save Data/Level_Stage_Prototype_GameObjects_230112.json", L"../Bin/Save Data/Level_Stage_CloneObjects_230127.json")), );

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Logo::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	SetWindowText(g_hWnd, L"Level : Logo");

	return S_OK;
}

HRESULT CLevel_Logo::Load_CloneObjects(const wstring & wstrCloneObjFilePath)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Json		jLayers;

	ifstream	file(wstrCloneObjFilePath.c_str());
	file >> jLayers;
	file.close();

	for (auto jLayer : jLayers["Layers"])
	{
		string		strLayerTag = "";
		wstring	wstrLayerTag = L"";
		jLayer["Layer Tag"].get_to<string>(strLayerTag);

		if (strLayerTag == "Layer_Enemies" || strLayerTag == "Layer_NPCs")
			continue;

		wstrLayerTag.assign(strLayerTag.begin(), strLayerTag.end());

		for (auto jCloneObj : jLayer["Clone Objects"])
		{
			string		strPrototypeObjTag = "";
			wstring	wstrPrototypeObjTag = L"";
			jCloneObj["Prototype GameObject Tag"].get_to<string>(strPrototypeObjTag);

			wstrPrototypeObjTag.assign(strPrototypeObjTag.begin(), strPrototypeObjTag.end());

			_float4x4		matWorld;
			XMStoreFloat4x4(&matWorld, XMMatrixIdentity());

			_uint	k = 0;
			for (_float fElement : jCloneObj["Transform State"])
				memcpy(((_float*)&matWorld) + (k++), &fElement, sizeof(_float));

			pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, wstrPrototypeObjTag, matWorld);
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Light()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::LIGHT_DIRECTIONAL;
	LightDesc.bIsLightOn = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	FAILED_CHECK_RETURN(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera::CAMERADESC		CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float4(30.f, 10.f, 0.f, 1.f);
	CameraDesc.vAt = _float4(50.f, 0.f, 30.f, 1.f);
	CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
	CameraDesc.fFov = XMConvertToRadians(75.f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.TransformDesc.dSpeedPerSec = 15.0;
	CameraDesc.TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_Camera_Dynamic", &CameraDesc), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_Logo"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_SkyBox(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_SkyBox"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Ocean(const wstring & wstrLayerTag)
{
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_Ocean"), E_FAIL);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Player(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_Sigrid_CutScene"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Islands(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_Portal_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_Mystic_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_River_Island"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrCloneObjFilePath)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize(wstrCloneObjFilePath)))
	{
		MSG_BOX("Failed to Create : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	CGameInstance::GetInstance()->Stop_All_Sound();

	__super::Free();
}
