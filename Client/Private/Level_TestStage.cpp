#include "stdafx.h"
#include "..\Public\Level_TestStage.h"
#include "GameInstance.h"

CLevel_TestStage::CLevel_TestStage(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_TestStage::Initialize()
{
	CGameInstance*		pInstance = CGameInstance::GetInstance();
	Safe_AddRef(pInstance);

	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	FAILED_CHECK_RETURN(pInstance->CreateScene(PxVec3(1.f, 1.f, 1.f)), E_FAIL);
	
	FAILED_CHECK_RETURN(Ready_Light(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_BackGround(L"Layer_BackGround"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Camera(L"Layer_Camera"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Sphere(L"Layer_Sphere"), E_FAIL);

	Safe_Release(pInstance);

	return S_OK;
}

void CLevel_TestStage::Tick(_double dTimeDelta)
{
	CGameInstance*		pInstance = CGameInstance::GetInstance();
	Safe_AddRef(pInstance);
	
	if (m_pPshycisScene == nullptr)
	{
		m_pPshycisScene = pInstance->Get_PxScene();
		pInstance->CreateSimulation();
	}

	pInstance->RunSimulation();

	Safe_Release(pInstance);

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
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	FAILED_CHECK_RETURN(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_BackGround(const wstring wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Terrain"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Camera(const wstring wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Camera_Dynamic"), E_FAIL);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TestStage::Ready_Layer_Sphere(const wstring wstrLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(pGameInstance->Clone_GameObject(LEVEL_TESTSTAGE, wstrLayerTag, L"Prototype_GameObject_Sphere"), E_FAIL);

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
