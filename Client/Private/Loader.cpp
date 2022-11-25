#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Texture.h"
#include "Terrain.h"
#include "MySphere.h"
#include "Model.h"
#include "Player.h"

CLoader::CLoader(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint	APIENTRY		LoadingThread(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevel())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;

	case LEVEL_TESTSTAGE:
		pLoader->Loading_ForGamePlay();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_wstrLoadingText = L"Loading Textures...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Logo", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Logo_1K.png")), E_FAIL);

	m_wstrLoadingText = L"Loading Buffer...";
	m_wstrLoadingText = L"Loading Models...";
	m_wstrLoadingText = L"Loading Shader...";
	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_BackGround", CBackGround::Create(m_pDevice, m_pContext)), E_FAIL);
	m_wstrLoadingText = L"Complete Loading!";

	m_bIsFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_wstrLoadingText = L"Loading Textures...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Terrain", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Test_%d.png", 2)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Brush", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Brush.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Filter", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Filter.bmp")), E_FAIL);

	m_wstrLoadingText = L"Loading Buffer...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../Bin/Resource/Height1.bmp")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_VIBuffer_Sphere", CVIBuffer_Sphere::Create(m_pDevice, m_pContext)), E_FAIL);

	m_wstrLoadingText = L"Loading Models...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Container", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Container/Container.fbx")), E_FAIL);
	m_wstrLoadingText = L"Loading Shader...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Terrain", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNorTex.hlsl", VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNonAnim.hlsl", VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Terrain", CTerrain::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Sphere", CMySphere::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Player", CPlayer::Create(m_pDevice, m_pContext)), E_FAIL);

	m_wstrLoadingText = L"Complete Loading!";

	m_bIsFinished = true;

	Safe_Release(pGameInstance);
	return S_OK;
}

CLoader * CLoader::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, LEVEL eNextLevel)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
