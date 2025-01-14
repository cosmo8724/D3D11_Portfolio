#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "CustomGameObject.h"
#include "Json/json.hpp"
#include <fstream>

/* Skybox */
#include "SkyBox.h"
#include "NewOcean.h"

/* Player */
#include "Sigrid.h"
#include "Sigrid_CutScn.h"
#include "Hat_CrabbyHat.h"
#include "Hat_FuzzyEars.h"
#include "Hat_JellyCorne.h"
#include "Hat_ToothyHood.h"

/* NPCs */
#include "Baine.h"
#include "Andy.h"
#include "Cordelia_Klout.h"
#include "Grandma.h"
#include "Klemens_Klout.h"
#include "Hollow_Knight.h"

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
#include "Monster_Black.h"
#include "Monster_Orange.h"
#include "Monster_Pink.h"
#include "Monster_White.h"

/* UI */
#include "Logo.h"
#include "HPBar.h"
#include "MonsterDrink_Frame.h"
#include "MonsterDrink_Icon.h"
#include "Shop_BackGround.h"
#include "Shop_MonsterCounter.h"
#include "Shop_Button1.h"
#include "Shop_Button2.h"
#include "Shop_ButtonA.h"
#include "Shop_ButtonD.h"
#include "Shop_ButtonW.h"
#include "Shop_ButtonS.h"
#include "Shop_ButtonQ.h"
#include "Shop_ButtonE.h"
#include "Shop_TurnLeft.h"
#include "Shop_TurnRight.h"
#include "Shop_Menu_Cloth.h"
#include "Shop_Menu_Hair.h"
#include "Shop_Menu_Hat.h"
#include "Shop_ArrowUp.h"
#include "Shop_ArrowDown.h"
#include "Shop_ItemBar.h"
#include "Shop_ItemSelect.h"
#include "Shop_Icon_Cloth.h"
#include "Shop_Icon_Hair.h"
#include "Shop_Icon_Hat.h"
#include "HPBar_Leviathan.h"
#include "Talk_NPC_Background.h"
#include "Talk_NPC_Corner.h"
#include "Talk_NPC_NameBox.h"
#include "Talk_Sigrid_Background.h"
#include "Talk_NPC_Icon.h"
#include "Talk_Button1.h"
#include "Talk_Button2.h"
#include "Talk_ButtonE.h"
#include "Talk_Sigrid_Icon.h"

/* Effect */
#include "Effect_SigridDash.h"
#include "Effect_GroundSlam.h"
#include "Effect_PreLightning.h"
#include "Effect_Lightning.h"
#include "Effect_Cocoball.h"
#include "Trail_Sigrid_Foot.h"
#include "Trail_Sigrid_Hand.h"
#include "Trail_Sigrid_Net.h"
#include "Trail_Cocoball.h"

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
		pLoader->Loading_ForLogo(pLoader->Get_ProtoComFilePath(), pLoader->Get_ProtoObjFilePath());
		break;

	case LEVEL_TESTSTAGE:
		pLoader->Loading_ForGamePlay(pLoader->Get_ProtoComFilePath(), pLoader->Get_ProtoObjFilePath());
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel, const wstring & wstrProtoComFilePath, const wstring & wstrProtoObjFilePath)
{
	m_eNextLevel = eNextLevel;
	m_wstrProtoComFilePath = wstrProtoComFilePath;
	m_wstrProtoObjFilePath = wstrProtoObjFilePath;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo(const wstring & wstrProtoComFilePath, const wstring & wstrProtoObjFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (wstrProtoComFilePath == L"")
	{
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
	}
	else
	{
		Json	jLevel;

		ifstream	file(wstrProtoComFilePath.c_str());
		file >> jLevel;
		file.close();

		string	strLevel = jLevel["Level"];

		if (strLevel != "Logo")
			return E_FAIL;

		for (auto& Com : jLevel["Components"])
		{
			string		strComponentType = "", strComponentTag = "";
			wstring	wstrComponentTag = L"";

			Com["Type"].get_to<string>(strComponentType);
			Com["Tag"].get_to<string>(strComponentTag);
			wstrComponentTag.assign(strComponentTag.begin(), strComponentTag.end());

			if (strComponentType == "Renderer")
			{
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CRenderer::Create(m_pDevice, m_pContext));
				continue;
			}
			else if (strComponentType == "VIBuffer_Sphere")
			{
				m_wstrLoadingText = L"Loading Buffer...";
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CVIBuffer_Sphere::Create(m_pDevice, m_pContext));
				continue;
			}
			/*else if (strComponentType == "VIBuffer_Terrain")
			{
				m_wstrLoadingText = L"Loading Buffer...";
				string		strFilePath = "";
				wstring	wstrFilePath = L"";

				Com["File Path"].get_to<string>(strFilePath);
				wstrFilePath.assign(strFilePath.begin(), strFilePath.end());

				CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CVIBuffer_Terrain::Create(m_pDevice, m_pContext, wstrFilePath));
				continue;
			}*/
			else if (strComponentType == "VIBuffer_Point_Instancing")
			{
				m_wstrLoadingText = L"Loading Buffer...";
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CVIBuffer_PointInstancing::Create(m_pDevice, m_pContext));
				continue;
			}
			else if (strComponentType == "Shader")
			{
				m_wstrLoadingText = L"Loading Shader...";
				string		strFilePath = "";
				wstring	wstrFilePath = L"";
				string		strDeclarationType = "";
				wstring	wstrDeclarationType = L"";
				_uint		iVertexElementsCount = 0;

				Com["File Path"].get_to<string>(strFilePath);
				wstrFilePath.assign(strFilePath.begin(), strFilePath.end());
				Com["Vertex Declaration"].get_to<string>(strDeclarationType);
				wstrDeclarationType.assign(strDeclarationType.begin(), strDeclarationType.end());
				Com["Vertex Elements Count"].get_to<_uint>(iVertexElementsCount);

				if (strDeclarationType == "Vtx_Position")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXPOS, VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_Texture")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXTEX, VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NormalTexture")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXNORTEX, VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NormalTexture_Ocean")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXNORTEX, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NormalTexture_NewOcean")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXNORTEX, VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NonAnimModel")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXMODEL, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_AnimModel")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXANIMMODEL, VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements));
				else
					continue;

				continue;
			}
			else if (strComponentType == "Transform")
			{
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CTransform::Create(m_pDevice, m_pContext));
				continue;
			}
			else if (strComponentType == "Texture")
			{
				m_wstrLoadingText = L"Loading Textures...";
				string		strFilePath = "";
				wstring	wstrFilePath = L"";
				_uint		iTextureCount = 0;

				Com["File Path"].get_to<string>(strFilePath);
				wstrFilePath.assign(strFilePath.begin(), strFilePath.end());
				Com["Texture Count"].get_to<_uint>(iTextureCount);

				CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CTexture::Create(m_pDevice, m_pContext, wstrFilePath, iTextureCount));
				continue;
			}
			else if (strComponentType == "Model")
			{
				m_wstrLoadingText = L"Loading Models...";
				string		strFilePath = "";
				string		strModelType = "";
				wstring	wstrModelType = L"";

				Com["File Path"].get_to<string>(strFilePath);
				Com["Model Type"].get_to<string>(strModelType);
				wstrModelType.assign(strModelType.begin(), strModelType.end());

				if (strModelType == "NonAnim")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, strFilePath.c_str(), XMMatrixIdentity()));
				else if (strModelType == "Anim")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, strFilePath.c_str(), XMMatrixIdentity()));
				else
					continue;
			}
			else if (strComponentType == "Collider")
			{
				string		strColliderType = "";
				wstring	wstrColliderType = L"";

				Com["Collider Type"].get_to<string>(strColliderType);
				wstrColliderType.assign(strColliderType.begin(), strColliderType.end());

				if (strColliderType == "Sphere")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_SPHERE));
				else if (strColliderType == "AABB")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_AABB));
				else if (strColliderType == "OBB")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_LOGO, wstrComponentTag, CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_OBB));
				else
					continue;
			}
		}

		jLevel.clear();
	}

	/* Testing */
	//FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_NewOcean_Foam", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Caustic/watter_effect.png")), E_FAIL);
	//FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_NewOcean_Height", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Caustic/waterbump.png")), E_FAIL);
	//FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_NewOcean", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_Ocean.hlsl", CShader::DECLARATION_VTXNORTEX, VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements)), E_FAIL);
	//FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Point_Instancing", CVIBuffer_PointInstancing::Create(m_pDevice, m_pContext)), E_FAIL);

	/*FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Shader_Ocean", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_VtxNorTex_Ocean.hlsl", CShader::DECLARATION_VTXNORTEX, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/HeightMap2.bmp")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/Caustics_tex_color.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Foam", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/Foam_Red.png")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean_Height", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Scene_Public/Caustics/HeightMap2.bmp")), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Ocean", COcean::Create(m_pDevice, m_pContext)), E_FAIL);*/

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Sigrid_CutScene", CSigrid_CutScn::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Logo", CLogo::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Portal_Island", CPortal_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Mystic_Island", CMystic_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_River_Island", CRiver_Island::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NewOcean", CNewOcean::Create(m_pDevice, m_pContext)), E_FAIL);

	if (wstrProtoObjFilePath != L"")
	{
		Json	jPrototypeObjects;

		ifstream	file(wstrProtoObjFilePath.c_str());
		file >> jPrototypeObjects;
		file.close();

		for (auto jObj : jPrototypeObjects["Objects"])
		{
			string		strObjectTag;
			wstring	wstrObjectTag;
			_uint		iTextureComCount = 1000;
			vector<pair<_uint, wstring>>	vecComponentInfo;

			jObj["Prototype Object Tag"].get_to<string>(strObjectTag);
			wstrObjectTag.assign(strObjectTag.begin(), strObjectTag.end());
			jObj["Texture Component Count"].get_to<_uint>(iTextureComCount);

			for (auto jCom : jObj["Components"])
			{
				_uint		iComponentLevelIndex = 1000;
				string		strComponentTag = "";
				wstring	wstrComponentTag = L"";

				jCom["Level"].get_to<_uint>(iComponentLevelIndex);
				jCom["Component Tag"].get_to<string>(strComponentTag);
				wstrComponentTag.assign(strComponentTag.begin(), strComponentTag.end());

				vecComponentInfo.push_back(pair<_uint, wstring>(iComponentLevelIndex, wstrComponentTag));
			}

			CGameInstance::GetInstance()->Add_Prototype(wstrObjectTag, CCustomGameObject::Create(m_pDevice, m_pContext, vecComponentInfo));
		}
	}

	m_wstrLoadingText = L"Complete Loading!";

	m_bIsFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay(const wstring & wstrProtoComFilePath, const wstring & wstrProtoObjFilePath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (wstrProtoComFilePath == L"")
	{
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
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Monster_Black", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Objects/Monster Black/Monster_Black.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Monster_Orange", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Objects/Monster Orange/Monster_Orange.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Monster_Pink", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Objects/Monster Pink/Monster_Pink.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Monster_White", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Objects/Monster White/Monster_White.model", XMMatrixIdentity())), E_FAIL);

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
	}
	else
	{
		Json	jLevel;

		ifstream	file(wstrProtoComFilePath.c_str());
		file >> jLevel;
		file.close();

		string	strLevel = jLevel["Level"];

		if (strLevel != "Test Stage")
			return E_FAIL;

		for (auto& Com : jLevel["Components"])
		{
			string		strComponentType = "", strComponentTag = "";
			wstring	wstrComponentTag = L"";

			Com["Type"].get_to<string>(strComponentType);
			Com["Tag"].get_to<string>(strComponentTag);
			wstrComponentTag.assign(strComponentTag.begin(), strComponentTag.end());

			if (strComponentType == "Renderer")
			{
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CRenderer::Create(m_pDevice, m_pContext));
				continue;
			}
			else if (strComponentType == "VIBuffer_Sphere")
			{
				m_wstrLoadingText = L"Loading Buffer...";
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CVIBuffer_Sphere::Create(m_pDevice, m_pContext));
				continue;
			}
			else if (strComponentType == "VIBuffer_Terrain")
			{
				m_wstrLoadingText = L"Loading Buffer...";
				string		strFilePath = "";
				wstring	wstrFilePath = L"";

				Com["File Path"].get_to<string>(strFilePath);
				wstrFilePath.assign(strFilePath.begin(), strFilePath.end());

				CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CVIBuffer_Terrain::Create(m_pDevice, m_pContext, wstrFilePath));
				continue;
			}
			else if (strComponentType == "Shader")
			{
				m_wstrLoadingText = L"Loading Shader...";
				string		strFilePath = "";
				wstring	wstrFilePath = L"";
				string		strDeclarationType = "";
				wstring	wstrDeclarationType = L"";
				_uint		iVertexElementsCount = 0;

				Com["File Path"].get_to<string>(strFilePath);
				wstrFilePath.assign(strFilePath.begin(), strFilePath.end());
				Com["Vertex Declaration"].get_to<string>(strDeclarationType);
				wstrDeclarationType.assign(strDeclarationType.begin(), strDeclarationType.end());
				Com["Vertex Elements Count"].get_to<_uint>(iVertexElementsCount);

				if (strDeclarationType == "Vtx_Position")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXPOS, VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_Texture")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXTEX, VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NormalTexture")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXNORTEX, VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NormalTexture_Ocean")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXNORTEX, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_NonAnimModel")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXMODEL, VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements));
				else if (strDeclarationType == "Vtx_AnimModel")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CShader::Create(m_pDevice, m_pContext, wstrFilePath, CShader::DECLARATION_VTXANIMMODEL, VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements));
				else
					continue;

				continue;
			}
			else if (strComponentType == "Transform")
			{
				CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CTransform::Create(m_pDevice, m_pContext));
				continue;
			}
			else if (strComponentType == "Texture")
			{
				m_wstrLoadingText = L"Loading Textures...";
				string		strFilePath = "";
				wstring	wstrFilePath = L"";
				_uint		iTextureCount = 0;

				Com["File Path"].get_to<string>(strFilePath);
				wstrFilePath.assign(strFilePath.begin(), strFilePath.end());
				Com["Texture Count"].get_to<_uint>(iTextureCount);

				CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CTexture::Create(m_pDevice, m_pContext, wstrFilePath, iTextureCount));
				continue;
			}
			else if (strComponentType == "Model")
			{
				m_wstrLoadingText = L"Loading Models...";
				string		strFilePath = "";
				string		strModelType = "";
				wstring	wstrModelType = L"";

				Com["File Path"].get_to<string>(strFilePath);
				Com["Model Type"].get_to<string>(strModelType);
				wstrModelType.assign(strModelType.begin(), strModelType.end());

				if (strModelType == "NonAnim")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, strFilePath.c_str(), XMMatrixIdentity()));
				else if (strModelType == "Anim")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, strFilePath.c_str(), XMMatrixIdentity()));
				else
					continue;
			}
			else if (strComponentType == "Collider")
			{
				m_wstrLoadingText = L"Loading Collider...";
				string		strColliderType = "";
				wstring	wstrColliderType = L"";

				Com["Collider Type"].get_to<string>(strColliderType);
				wstrColliderType.assign(strColliderType.begin(), strColliderType.end());

				if (strColliderType == "Sphere")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_SPHERE));
				else if (strColliderType == "AABB")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_AABB));
				else if (strColliderType == "OBB")
					CGameInstance::GetInstance()->Add_Prototype(LEVEL_TESTSTAGE, wstrComponentTag, CCollider::Create(m_pDevice, m_pContext, CCollider::COLLIDER_OBB));
				else
					continue;
			}
		}

		jLevel.clear();

		/* Testing */
		/*FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_HPBar", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Health Bar/HpBar.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Frame", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Monster Drink Icon/MonsterDrink_Frame.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_Black", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Monster Drink Icon/MonsterDrink_Black.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_Orange", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Monster Drink Icon/MonsterDrink_Orange.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_Pink", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Monster Drink Icon/MonsterDrink_Pink.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_White", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Monster Drink Icon/MonsterDrink_White.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Sigrid_Dash", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Sigrid Dash/Sigrid_Dash.model", XMMatrixScaling(0.01f, 0.01f, 0.01f))), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_WhiteSRC", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Mesh/Effect/Base_White.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_BackGround", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShopUI_mockup_BG.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_MonsterCounter", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShop_UI_SparkCounter.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Button1", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_Alpha1.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Button2", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_Alpha2.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonA", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_A.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonD", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_D.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonW", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_W.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonS", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_S.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonQ", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_Q.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonE", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/GUI_InputSpriteAtlas_Keyboard_E.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ArrowUp", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/Arrow_Up.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ArrowDown", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/Arrow_Down.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_TurnLeft", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/Turn_Left.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_TurnRight", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/Turn_Right.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemBar", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/HUD_BainesShop_ItemBar_%d.png", 3)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemSelectL", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/HUD_BainesShop_SelectionBracketL.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemSelectR", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/HUD_BainesShop_SelectionBracketR.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_SelectBarL", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/HUD_BainesShop_SelectionBracketL.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_SelectBarR", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/HUD_BainesShop_SelectionBracketR.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Menu_Cloth", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShop_UI_Clothes_%d.png", 2)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Menu_Hair", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShop_UI_Hair_%d.png", 2)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Menu_Hat", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShop_UI_Hats_%d.png", 2)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemBar_Icon_Cloth", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShopUI_mockup_ItemIcon_PatternSel.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemBar_Icon_Hair", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShopUI_mockup_ItemIcon_HairSel.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemBar_Icon_Hat", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Shop/BainesShopUI_mockup_ItemIcon_HatsSel.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_Stripes", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Skin/Outfit/Cosmetic_OutfitStripes_BC.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_FlowerPower", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Skin/Outfit/Cosmetic_OutfitFlowerPower_BC.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_Camouflage", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Skin/Outfit/Cosmetic_OutfitCamouflage_BC.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_Hearts", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Skin/Outfit/Cosmetic_OutfitHearts_BC.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_HairMask", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Skin/Hair/SigridHair_BC.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_GroundSlamCircle", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Effect/T_Soundwave.png")), E_FAIL);

		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hat_CrabbyHat", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Sigrid/Hat/CrabbyHat/CrabbyHat.fbx", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hat_FuzzyEars", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Sigrid/Hat/FuzzyEars/FuzzyEars.fbx", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hat_JellyCorne", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Sigrid/Hat/JellyCorne/JellyCorne.fbx", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hat_ToothyHood", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Sigrid/Hat/ToothyHood/ToothyHood.fbx", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Circle", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Ring.fbx", XMMatrixIdentity())), E_FAIL);*/

		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_PreLighting", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Effect/PreLightning.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_Lighting", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Effect/Lightning.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Cross", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Cross.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f))), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Lightning_1", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Lightning_1.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f))), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Lightning_2", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Lightning_2.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f))), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Trail", CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_Trail.hlsl", CShader::DECLARATION_VTXPOINTINSTANCING, VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Cocoball", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Sphere.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f))), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_Cocoball", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/Effect/causticF.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_HalfCylinder", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_NONANIM, "../Bin/Resource/Mesh/Effect/Half_Cylinder.fbx", XMMatrixScaling(0.01f, 0.01f, 0.01f))), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_HPBar_Boss", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Health Bar/HPBar_Boss.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_HPBar_Boss_Frame", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Health Bar/HPBar_Boss_Frame.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_VIBuffer_Trail", CVIBuffer_PointInstancing::Create(m_pDevice, m_pContext, 300)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Sigrid_NetRing", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Mesh/Sigrid/SigridNet_BC.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Andy", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/NPCs/Andy/Andy.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Cordelia_Klout", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/NPCs/CordeliaKlout/CordeliaKlout.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Grandma", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/NPCs/Grandma/Grandma.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Klemens_Klout", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/NPCs/KlemensKlout/KlemensKlout.model", XMMatrixIdentity())), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_NPC_BackGround", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Talk/UI_elements_Dialogbubble1.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_NPC_Corner", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Talk/UI_elements_DialogBubbleCorners.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_NPC_NameBox", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Talk/HUD_DialogNamebox.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_Sigrid_BackGround", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Talk/ZTemp_DialogChoiseBackground.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_NPC_Icon", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Talk/NPC_Talk_%d.png", 5)), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_Sigrid_Icon", CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resource/Texture/UI/Talk/UI_elements_Sigrid.png")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Model_NPC_Hollow_Knight", CModel::Create(m_pDevice, m_pContext, CModel::MODEL_ANIM, "../Bin/Resource/Mesh/NPCs/Hollow Knight/Hollow_Knight.model", XMMatrixIdentity())), E_FAIL);

		m_wstrLoadingText = L"Loading Navigation Info...";
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Default.json")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Roof", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Roof.json")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Sky", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Sky.json")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Fantasy_Island", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Fantasy_Island.json")), E_FAIL);
		FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Hellheim", CNavigation::Create(m_pDevice, m_pContext, L"../Bin/Save Data/Navigation/Navigation_Hellheim.json")), E_FAIL);
	}

	m_wstrLoadingText = L"Create Prototype Objects...";
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Sigrid", CSigrid::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Baine", CBaine::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Andy", CAndy::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Grandma", CGrandma::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Cordelia_Klout", CCordelia_Klout::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Klemens_Klout", CKlemens_Klout::Create(m_pDevice, m_pContext)), E_FAIL);
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
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Black", CMonster_Black::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Orange", CMonster_Orange::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_Pink", CMonster_Pink::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Monster_White", CMonster_White::Create(m_pDevice, m_pContext)), E_FAIL);

	/* Testing */
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_HPBar", CHPBar::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Frame_Black", CMonsterDrink_Frame::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_BLACK)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Frame_Orange", CMonsterDrink_Frame::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_ORANGE)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Frame_Pink", CMonsterDrink_Frame::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_PINK)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Frame_White", CMonsterDrink_Frame::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_WHITE)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Icon_Black", CMonsterDrink_Icon::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_BLACK)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Icon_Orange", CMonsterDrink_Icon::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_ORANGE)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Icon_Pink", CMonsterDrink_Icon::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_PINK)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_MonsterDrink_Icon_White", CMonsterDrink_Icon::Create(m_pDevice, m_pContext, CMonsterDrink_Frame::MONSTERDRINK_WHITE)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_BackGround", CShop_BackGround::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_MonsterCounter", CShop_MonsterCounter::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_Button1", CShop_Button1::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_Button2", CShop_Button2::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ButtonA", CShop_ButtonA::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ButtonD", CShop_ButtonD::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ButtonW", CShop_ButtonW::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ButtonS", CShop_ButtonS::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ButtonQ", CShop_ButtonQ::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ButtonE", CShop_ButtonE::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ArrowUp", CShop_ArrowUp::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ArrowDown", CShop_ArrowDown::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_TurnLeft", CShop_TurnLeft::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_TurnRight", CShop_TurnRight::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_Menu_Cloth", CShop_Menu_Cloth::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_Menu_Hair", CShop_Menu_Hair::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_Menu_Hat", CShop_Menu_Hat::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ItemBar", CShop_ItemBar::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ItemSelectL", CShop_ItemSelect::Create(m_pDevice, m_pContext, CShop_ItemSelect::FRAME_LEFT)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ItemSelectR", CShop_ItemSelect::Create(m_pDevice, m_pContext, CShop_ItemSelect::FRAME_RIGHT)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ItemBar_Icon_Cloth", CShop_Icon_Cloth::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ItemBar_Icon_Hair", CShop_Icon_Hair::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Shop_ItemBar_Icon_Hat", CShop_Icon_Hat::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_Sigrid_Dash", CEffect_SigridDash::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Hat_CrabbyHat", CHat_CrabbyHat::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Hat_FuzzyEars", CHat_FuzzyEars::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Hat_JellyCorne", CHat_JellyCorne::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Hat_ToothyHood", CHat_ToothyHood::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_GroundSlam", CEffect_GroundSlam::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_PreLightning", CEffect_PreLightning::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_Lightning", CEffect_Lightning::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_Cocoball", CEffect_Cocoball::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_HPBar_Boss", CHPBar_Leviathan::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_Trail_Sigrid_Hand", CTrail_Sigrid_Hand::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_Trail_Sigrid_Net", CTrail_Sigrid_Net::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_Effect_Trail_Cocoball", CTrail_Cocoball::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_NPC_BackGround", CTalk_NPC_Background::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_NPC_Corner", CTalk_NPC_Corner::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_NPC_NameBox", CTalk_NPC_NameBox::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_NPC_Icon", CTalk_NPC_Icon::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_Sigrid_BackGround", CTalk_Sigrid_Background::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_Sigrid_Icon", CTalk_Sigrid_Icon::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_Button1", CTalk_Button1::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_Button2", CTalk_Button2::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_UI_Talk_ButtonE", CTalk_ButtonE::Create(m_pDevice, m_pContext)), E_FAIL);
	FAILED_CHECK_RETURN(pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Hollow_Knight", CHollow_Knight::Create(m_pDevice, m_pContext)), E_FAIL);

	if (wstrProtoObjFilePath != L"")
	{
		if (wstrProtoObjFilePath != L"")
		{
			Json	jPrototypeObjects;

			ifstream	file(wstrProtoObjFilePath.c_str());
			file >> jPrototypeObjects;
			file.close();

			for (auto jObj : jPrototypeObjects["Objects"])
			{
				string		strObjectTag;
				wstring	wstrObjectTag;
				_uint		iTextureComCount = 1000;
				vector<pair<_uint, wstring>>	vecComponentInfo;

				jObj["Prototype Object Tag"].get_to<string>(strObjectTag);
				wstrObjectTag.assign(strObjectTag.begin(), strObjectTag.end());
				jObj["Texture Component Count"].get_to<_uint>(iTextureComCount);

				for (auto jCom : jObj["Components"])
				{
					_uint		iComponentLevelIndex = 1000;
					string		strComponentTag = "";
					wstring	wstrComponentTag = L"";

					jCom["Level"].get_to<_uint>(iComponentLevelIndex);
					jCom["Component Tag"].get_to<string>(strComponentTag);
					wstrComponentTag.assign(strComponentTag.begin(), strComponentTag.end());

					vecComponentInfo.push_back(pair<_uint, wstring>(iComponentLevelIndex, wstrComponentTag));
				}

				CGameInstance::GetInstance()->Add_Prototype(wstrObjectTag, CCustomGameObject::Create(m_pDevice, m_pContext, vecComponentInfo));
			}
		}
	}

	m_wstrLoadingText = L"Complete Loading!";

	m_bIsFinished = true;

	Safe_Release(pGameInstance);
	return S_OK;
}

CLoader * CLoader::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, LEVEL eNextLevel, const wstring & wstrProtoComFilePath, const wstring & wstrProtoObjFilePath)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, wstrProtoComFilePath, wstrProtoObjFilePath)))
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
