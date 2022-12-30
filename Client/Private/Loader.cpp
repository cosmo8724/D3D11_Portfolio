#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "Logo.h"
#include "Terrain.h"
#include "MySphere.h"
#include "Player.h"
#include "Sword_Handle.h"
#include "Sword_Blade.h"
#include "Sigrid.h"
#include "SkyBox.h"
#include "MainIsland.h"

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
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Logo", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Logo/Logo/MainMenu_Logo.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_SkyBox", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/SkyBox/Skybox_01.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/Caustics_tex_color.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Foam", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/Foam_Red.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Height", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/HeightMap.bmp")), E_FAIL);

	m_wstrLoadingText = L"Loading Buffer...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Sphere", CVIBuffer_Sphere::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/HeightMap.bmp")), E_FAIL);

	m_wstrLoadingText = L"Loading Models...";

	m_wstrLoadingText = L"Loading Shader...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_Terrain", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNorTex.hlsl", CShader::DECLARATION_VTXNORTEX, VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_Ocean", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNorTex_Ocean.hlsl", CShader::DECLARATION_VTXNORTEX, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Logo", CLogo::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)), E_FAIL);

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

	m_wstrLoadingText = L"Loading Buffer...";

	m_wstrLoadingText = L"Loading Models...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Sigrid", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/Sigrid/Sigrid.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_MainIsland", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Main Island/MainIsland.model", XMMatrixIdentity())), E_FAIL);
	
	m_wstrLoadingText = L"Loading Collider...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_SPHERE)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_AABB", CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_AABB)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_OBB", CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_OBB)), E_FAIL);

	m_wstrLoadingText = L"Loading Shader...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNonAnim.hlsl", CShader::DECLARATION_VTXMODEL, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxAnimModel.hlsl", CShader::DECLARATION_VTXANIMMODEL, VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements)), E_FAIL);

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Sigrid", CSigrid::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_MainIsland", CMainIsland::Create(m_pDevice, m_pContext)), E_FAIL);

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
