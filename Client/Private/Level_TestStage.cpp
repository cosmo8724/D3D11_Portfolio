#include "stdafx.h"
#include "..\Public\Level_TestStage.h"
#include "GameInstance.h"
#include "Json/json.hpp"
#include <fstream>
#include "Enemy.h"
#include "Sigrid.h"
#include "Camera.h"
#include "NPC.h"
#include "MonsterDrink.h"

CLevel_TestStage::CLevel_TestStage(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_TestStage::Initialize(const wstring & wstrCloneObjFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	
	//FAILED_CHECK_RETURN(Ready_Light(), E_FAIL);

	if (wstrCloneObjFilePath == L"")
	{
		FAILED_CHECK_RETURN(Ready_Layer_Ocean(L"Layer_Ocean"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_Camera(L"Layer_Camera"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_SkyBox(L"Layer_SkyBox"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_Player(L"Layer_Player"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_NPCs(L"Layer_NPCs"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_Islands(L"Layer_Islands"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_Enemies(L"Layer_Enemies"), E_FAIL);

		FAILED_CHECK_RETURN(Ready_Layer_Objects(L"Layer_Objects"), E_FAIL);
	}
	else
		FAILED_CHECK_RETURN(Load_CloneObjects(wstrCloneObjFilePath), E_FAIL);

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

HRESULT CLevel_TestStage::Load_CloneObjects(const wstring & wstrCloneObjFilePath)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Json		jLayers;

	ifstream	file(wstrCloneObjFilePath.c_str());
	file >> jLayers;
	file.close();

	CSigrid*	pPlayer = nullptr;

	for (auto jLayer : jLayers["Layers"])
	{
		string		strLayerTag = "";
		wstring	wstrLayerTag = L"";
		jLayer["Layer Tag"].get_to<string>(strLayerTag);

		if (strLayerTag == "Layer_Enemies" || strLayerTag == "Layer_NPCs" || strLayerTag == "Layer_Coins")
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

			CGameObject*	pGameObject = pGameInstance->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, wstrLayerTag, wstrPrototypeObjTag, matWorld);
			if (dynamic_cast<CSigrid*>(pGameObject))
				pPlayer = dynamic_cast<CSigrid*>(pGameObject);
		}
	}

	for (auto jLayer : jLayers["Layers"])
	{
		string		strLayerTag = "";
		wstring	wstrLayerTag = L"";
		jLayer["Layer Tag"].get_to<string>(strLayerTag);

		if (strLayerTag != "Layer_Enemies" && strLayerTag != "Layer_NPCs" && strLayerTag != "Layer_Coins")
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

			CGameObject*	pGameObject = pGameInstance->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, wstrLayerTag, wstrPrototypeObjTag, matWorld);

			if (strLayerTag == "Layer_Enemies")
			{
				if (CEnemy*	pEnemy = dynamic_cast<CEnemy*>(pGameObject))
					pEnemy->Set_Player(pPlayer);
			}
			else if (strLayerTag == "Layer_NPCs")
			{
				if (CNPC*		pNPC = dynamic_cast<CNPC*>(pGameObject))
					pNPC->Set_Player(pPlayer);
			}
			else if (strLayerTag == "Layer_Coins")
			{
				if (CMonsterDrink*	pCoin = dynamic_cast<CMonsterDrink*>(pGameObject))
					pCoin->Set_Player(pPlayer);
			}
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Light()
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

HRESULT CLevel_TestStage::Ready_Layer_Ocean(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Ocean"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Camera(const wstring & wstrLayerTag)
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

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Camera_Dynamic", &CameraDesc), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Camera_Static", &CameraDesc), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_SkyBox(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_SkyBox"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Player(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Sigrid"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_NPCs(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_NPC_Baine"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Islands(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_MainIsland"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Portal_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Hellheim_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Fantasy_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Mystic_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_River_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Stylized_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Shop_Island"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Temple_Island"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Enemies(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CEnemy*		pEnemy = nullptr;
	CSigrid*		pPlayer = dynamic_cast<CSigrid*>(pGameInstance->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->front());
	NULL_CHECK_RETURN(pPlayer, E_FAIL);

	pEnemy = dynamic_cast<CEnemy*>(pGameInstance->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Critter_Small"));
	NULL_CHECK_RETURN(pEnemy, E_FAIL);
	pEnemy->Set_Player(pPlayer);

	/*pEnemy = dynamic_cast<CEnemy*>(pGameInstance->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Shinobi"));
	NULL_CHECK_RETURN(pEnemy, E_FAIL);
	pEnemy->Set_Player(pPlayer);*/

	pEnemy = dynamic_cast<CEnemy*>(pGameInstance->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Leviathan"));
	NULL_CHECK_RETURN(pEnemy, E_FAIL);
	pEnemy->Set_Player(pPlayer);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Objects(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Grapple_Launcher"), E_FAIL);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Grapple_Hang"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_TestStage * CLevel_TestStage::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrCloneObjFilePath)
{
	CLevel_TestStage*		pInstance = new CLevel_TestStage(pDevice, pContext);

	if (FAILED(pInstance->Initialize(wstrCloneObjFilePath)))
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
