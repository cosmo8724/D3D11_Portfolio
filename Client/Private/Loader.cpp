#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "Logo.h"

/* Skybox */
#include "SkyBox.h"

/* Player */
#include "Sigrid.h"
#include "Sigrid_CutScn.h"

/* NPCs */
#include "Baine.h"

/* Islands */
#include "MainIsland.h"
#include "Portal_Island.h"
#include "Hellheim_Island.h"
#include "Fantasy_Island.h"
#include "Mystic_Island.h"
#include "River_Island.h"
#include "Stylized_Island.h"
#include "Shop_Island.h"
#include "Temple_Island.h"

/* Enemies */
#include "Critter_Small.h"
#include "Shinobi.h"
#include "Leviathan.h"

/* Objects */
#include "Grapple_Launcher.h"
#include "Grapple_Hang.h"

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
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Height", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/HeightMap2.bmp")), E_FAIL);

	m_wstrLoadingText = L"Loading Buffer...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Sphere", CVIBuffer_Sphere::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/HeightMap2.bmp")), E_FAIL);

	m_wstrLoadingText = L"Loading Models...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Model_Sigrid_CutScene", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/Sigrid_CutScn/Sigrid_CutScene.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Model_Portal_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Portal Island/Portal_Island.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Model_Mystic_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Mystic Stones Of The Sky/Mystic_Stones_Of_The_Sky.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Model_River_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/River Island/River_Island.model", XMMatrixIdentity())), E_FAIL);

	m_wstrLoadingText = L"Loading Shader...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_NonAnim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNonAnim.hlsl", CShader::DECLARATION_VTXMODEL, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_Anim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxAnimModel.hlsl", CShader::DECLARATION_VTXANIMMODEL, VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_Terrain", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNorTex.hlsl", CShader::DECLARATION_VTXNORTEX, VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_Ocean", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNorTex_Ocean.hlsl", CShader::DECLARATION_VTXNORTEX, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Sigrid_CutScene", CSigrid_CutScn::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Logo", CLogo::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Portal_Island", CPortal_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Mystic_Island", CMystic_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_River_Island", CRiver_Island::Create(m_pDevice, m_pContext)), E_FAIL);

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
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Baine", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/NPCs/Baine/Baine.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_MainIsland", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Main Island/MainIsland.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hellheim_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Hellheim/Hellheim.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Fantasy_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Fantasy Islands/Fantasy_Island_Delete.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Stylized_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Stylized Island/Stylized_Island.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Shop_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Destiny Island/Destiny_Island.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Temple_Island", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Islands/Temple Island/Temple_Island.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Critter_Small", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/Enemy/Critter Small/Critter_Small.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Shinobi", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/Enemy/Shinobi/Shinobi.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Leviathan", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/Enemy/Leviathan/Leviathan.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Grapple_Launcher", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Objects/Grapple Launcher/Grapple_Launcher.model", XMMatrixIdentity())), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Grapple_Hang", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Objects/Grapple Hang/Grapple_Hang.model", XMMatrixIdentity())), E_FAIL);

	m_wstrLoadingText = L"Loading Collider...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_SPHERE)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_AABB", CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_AABB)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_OBB", CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_OBB)), E_FAIL);

	m_wstrLoadingText = L"Loading Navigation Info...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Default.json")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Roof", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Roof.json")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Sky", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Sky.json")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Fantasy_Island", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Fantasy_Island.json")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Hellheim", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Hellheim.json")), E_FAIL);

	m_wstrLoadingText = L"Loading Shader...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNonAnim.hlsl", CShader::DECLARATION_VTXMODEL, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxAnimModel.hlsl", CShader::DECLARATION_VTXANIMMODEL, VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements)), E_FAIL);

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Sigrid", CSigrid::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Baine", CBaine::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_MainIsland", CMainIsland::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Hellheim_Island", CHellheim_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Fantasy_Island", CFantasy_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Stylized_Island", CStylized_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Shop_Island", CShop_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Temple_Island", CTemple_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Critter_Small", CCritter_Small::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Shinobi", CShinobi::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Leviathan", CLeviathan::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Grapple_Launcher", CGrapple_Launcher::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Grapple_Hang", CGrapple_Hang::Create(m_pDevice, m_pContext)), E_FAIL);

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
