#include "stdafx.h"
#include "..\Public\Level_TestStage.h"
#include "GameInstance.h"
#include "Enemy.h"
#include "Sigrid.h"

CLevel_TestStage::CLevel_TestStage(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_TestStage::Initialize()
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);
	
	//FAILED_CHECK_RETURN(Ready_Light(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Ocean(L"Layer_Ocean"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Camera(L"Layer_Camera"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_SkyBox(L"Layer_SkyBox"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Player(L"Layer_Player"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_NPCs(L"Layer_NPCs"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Islands(L"Layer_Islands"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Enemies(L"Layer_Enemies"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Objects(L"Layer_Objects"), E_FAIL);

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

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Camera_Dynamic"), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Camera_Static"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_SkyBox(const wstring & wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_LOGO, wstrLayerTag, L"Prototype_GameObject_SkyBox"), E_FAIL);

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
