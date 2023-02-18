#include "stdafx.h"
#include "..\Public\Sigrid.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Static_Camera.h"
#include "Sigrid_State.h"
#include "Ocean.h"
#include "Enemy.h"
#include "Shop_BackGround.h"
#include "Trail_Sigrid_Foot.h"
#include "Trail_Sigrid_Hand.h"
#include "Trail_Sigrid_Net.h"

CSigrid::CSigrid(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
	, m_fGravity(0.02f)
	, m_fInitJumpSpeed(1.5f)
	, m_fCurJumpSpeed(0.f)
	, m_fFriction(0.1f)
	, m_fMaxDashTickCount(1.f)
	, m_fCurDashTickCount(0.f)
{
}

CSigrid::CSigrid(const CSigrid & rhs)
	: CGameObject(rhs)
	, m_fGravity(rhs.m_fGravity)
	, m_fInitJumpSpeed(rhs.m_fInitJumpSpeed)
	, m_fCurJumpSpeed(rhs.m_fCurJumpSpeed)
	, m_fFriction(rhs.m_fFriction)
	, m_fMaxDashTickCount(rhs.m_fMaxDashTickCount)
	, m_fCurDashTickCount(rhs.m_fCurDashTickCount)
{
	ZeroMemory(m_vHairColor, sizeof(_float4) * 4);
}

_uint CSigrid::Get_EquipSlot(_uint iMenuIndex)
{
	for (_uint i = 0; i < 5; ++i)
	{
		if (m_eItemState[iMenuIndex][i] == ITEM_EQUIPPED)
			return i;
	}

	return 10000;
}

_float4 CSigrid::Get_CurrentHairColor()
{
	_float4	vColor = { 0.f, 0.f, 0.f, 0.f };

	if (m_bHairMask == true)
	{
		if (m_iCurrentHair != 0)
			vColor = m_vHairColor[m_iCurrentHair - 1];

		if (g_bShopOpen == true)
		{
			if (m_iPreviewHair != 0)
				vColor = m_vHairColor[m_iPreviewHair - 1];
			else
			{
				if (m_iCurrentHair != 0)
					vColor = m_vHairColor[m_iCurrentHair - 1];
				else
					vColor = { 0.f, 0.f, 0.f, 0.f };
			}
		}
	}

	return vColor;
}

void CSigrid::EquipItem(_uint iMenuIndex, _uint iSlot)
{
	switch (iMenuIndex)
	{
	case 0:
		Set_CurrentOutfit(iSlot);
		break;

	case 1:
		Set_CurrentHair(iSlot);
		break;

	case 2:
		Set_CurrentHat(iSlot);
		break;
	}
}

HRESULT CSigrid::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSigrid::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 15.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(195.5f, 3.26f, 267.f, 1.f));

	/* Set Camera */
	m_pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	m_pCamera->Set_OwnerTransform(m_pTransformCom);
	m_pCamera->Init_Position();

	/* Set Friend State class */
	m_pSigridState = CSigrid_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom, m_pCamera);
	NULL_CHECK_RETURN(m_pSigridState, E_FAIL);

	m_pModelCom->Set_CurAnimationIndex(CSigrid_State::GROUND_IDLE);

	m_tStatus.iMaxHP = 100;
	m_tStatus.iHP = m_tStatus.iMaxHP;
	m_tStatus.iAttack = 7;
	m_tStatus.iSpecialAttack = 10;
	m_tStatus.dInitHitCoolTime = 2.0;
	m_tStatus.dCurHitCoolTime = 0.0;

	m_vHairColor[0] = FLOAT4COLOR_RGBA(253.f, 229.f, 169.f, 255.f);
	m_vHairColor[1] = FLOAT4COLOR_RGBA(255.f, 255.f, 255.f, 255.f);
	m_vHairColor[2] = FLOAT4COLOR_RGBA(241.f, 116.f, 115.f, 255.f);
	m_vHairColor[3] = FLOAT4COLOR_RGBA(68.f, 27.f, 38.f, 255.f);

	FAILED_CHECK_RETURN(SetUp_Parts_Hat(), E_FAIL);

	m_pLeftHandTrail = dynamic_cast<CTrail_Sigrid_Hand*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Trail_Sigrid_Hand"));
	m_pRightHandTrail = dynamic_cast<CTrail_Sigrid_Hand*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Trail_Sigrid_Hand"));
	m_pNetTrail = dynamic_cast<CTrail_Sigrid_Net*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Trail_Sigrid_Net"));

	return S_OK;
}

void CSigrid::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (g_bShopOpen == false)
	{
		Check_CurrentNavigation();

		if (m_pNavigationCom[m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_OCEAN)
			m_bOnOcean = true;
		else if (m_pNavigationCom[m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_GROUND || m_pNavigationCom[m_eCurNavigation]->Get_CurrentCellState() == CCell::STATE_ROOF)
			m_bOnOcean = false;

		if (m_bOnOcean == true)
			SetOn_Terrain();
		else
			SetOn_Navigation();

		m_pSigridState->Tick(dTimeDelta);
		m_pStateMachineCom->Tick(dTimeDelta);

		m_iPreviewOutfit = 0;
		m_iPreviewHair = 0;
		m_iPreviewHat = 0;
	}

	CGameInstance::GetInstance()->Set_TimeScale(L"Timer_165", dTimeDelta, 1.0);
	m_pModelCom->Play_Animation(dTimeDelta, m_eLerpType);
	CGameInstance::GetInstance()->Set_TimeScale(L"Timer_165", dTimeDelta, m_dTimeScale);

	m_pOBBCol->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
	m_pSphereCol->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	_matrix	matNetRing = m_pModelCom->Get_BoneMatrix("NetRing DEF 1");
	m_pNetSphereCol->Update(matNetRing * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	CGameInstance::GetInstance()->Set_LightPosition(2, m_pTransformCom->Get_State(CTransform::STATE_TRANS));

	for (auto& pGameObject : m_vecHats)
		pGameObject->Tick(dTimeDelta);

	_float4x4		matLeftHand = m_pModelCom->Get_BoneMatrix("Sigrid DEF L Arm Middle 3") * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrixXMMatrix();
	m_pLeftHandTrail->Set_WorldMatrix(matLeftHand);

	_float4x4		matRightHand = m_pModelCom->Get_BoneMatrix("Sigrid DEF R Arm Middle 3") * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrixXMMatrix();
	m_pRightHandTrail->Set_WorldMatrix(matRightHand);

	m_pNetTrail->Set_WorldMatrix(matNetRing * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrixXMMatrix());
	m_pNetTrail->Set_Active(m_bAttack);
}

void CSigrid::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pSigridState->Late_Tick(dTimeDelta);

	if (m_iCurrentHair != 0 || m_iPreviewHair != 0)
	{
		m_bHairMask = true;

		if (g_bShopOpen == false && g_bReadySceneChange == false && m_iCurrentHair != 0)
			m_pNetTrail->Set_Color(m_vHairColor[m_iCurrentHair - 1]);

		if (g_bShopOpen == true && dynamic_cast<CShop_BackGround*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_UI")->back())->Get_CurrentMenu() == 1 && m_iPreviewHair == 0)
			m_bHairMask = false;
	}
	else
	{
		m_bHairMask = false;

		if (g_bShopOpen == false && g_bReadySceneChange == false)
			m_pNetTrail->Set_Color(FLOAT4COLOR_RGBA(53, 238, 255, 255));
	}

	if (nullptr != m_pRendererCom && g_bReadySceneChange == false)
	{
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		//m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
		//m_pRendererCom->Add_DebugRenderGroup(m_pOBBCol);
		//m_pRendererCom->Add_DebugRenderGroup(m_pNetSphereCol);
		//m_pRendererCom->Add_DebugRenderGroup(m_pNavigationCom);
	}

	if (m_iCurrentHat != 0)
	{
		if (m_iPreviewHat == 0)
			m_vecHats[m_iCurrentHat - 1]->Late_Tick(dTimeDelta);
	}

	if (m_iPreviewHat != 0)
	{
		m_vecHats[m_iPreviewHat - 1]->Late_Tick(dTimeDelta);
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_F8))
	{
		_matrix	matTemp = XMMatrixIdentity();
		memcpy(&matTemp.r[3], &m_pTransformCom->Get_State(CTransform::STATE_TRANS), sizeof(_float4));
		matTemp.r[3] = XMVectorSetY(matTemp.r[3], XMVectorGetY(matTemp.r[3]) + 4.f);

		//CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_PreLightning", matTemp);
		CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_UI_HPBar_Boss", matTemp);
	}
}

HRESULT CSigrid::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 4)
			m_pNetRingTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
		else
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");

		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		if (m_iCurrentOutfit != 0)
		{
			if (i < 2)
			{
				m_pOutfitTextureCom[m_iCurrentOutfit - 1]->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
				
				if (g_bShopOpen == true && dynamic_cast<CShop_BackGround*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_UI")->back())->Get_CurrentMenu() == 0 && m_iPreviewOutfit == 0)
					m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
			}
		}

		if (m_iPreviewOutfit != 0)
		{
			if (i < 2)
				m_pOutfitTextureCom[m_iPreviewOutfit - 1]->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
		}

		if (m_bHairMask == false)
			m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 1);
		else
		{
			if (i < 2)
			{
				if (m_iPreviewHair != 0)
					m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 3);
				else
					m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 3);
			}
			else
				m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 1);
		}
	}

	if (g_bShopOpen == true || g_bReadySceneChange == true)
	{
		if (m_iPreviewHat != 0)
			m_vecHats[m_iPreviewHat - 1]->Render();
		else
		{
			if (m_iCurrentHat != 0 && dynamic_cast<CShop_BackGround*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_UI")->back())->Get_CurrentMenu() != 2)
				m_vecHats[m_iCurrentHat - 1]->Render();
		}
	}

	return S_OK;
}

HRESULT CSigrid::Render_ShadowDepth()
{
	FAILED_CHECK_RETURN(__super::Render_ShadowDepth(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource_LightDepth(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		//m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		//m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 2);
	}

	return S_OK;
}

HRESULT CSigrid::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		if (m_iCurrentOutfit != 0)
		{
			if (i < 2)
			{
				m_pOutfitTextureCom[m_iCurrentOutfit - 1]->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");

				if (g_bShopOpen == true && dynamic_cast<CShop_BackGround*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_UI")->back())->Get_CurrentMenu() == 0 && m_iPreviewOutfit == 0)
					m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
			}
		}

		if (m_iPreviewOutfit != 0)
		{
			if (i < 2)
				m_pOutfitTextureCom[m_iPreviewOutfit - 1]->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
		}

		if (m_bHairMask == false)
			m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 4);
		else
		{
			if (i < 2)
			{
				if (m_iPreviewHair != 0)
					m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 5);
				else
					m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 5);
			}
			else
				m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 4);
		}
	}

	if (g_bShopOpen == true || g_bReadySceneChange == true)
	{
		if (m_iPreviewHat != 0)
			m_vecHats[m_iPreviewHat - 1]->Render_Reflect();
		else
		{
			if (m_iCurrentHat != 0 && dynamic_cast<CShop_BackGround*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_UI")->back())->Get_CurrentMenu() != 2)
				m_vecHats[m_iCurrentHat - 1]->Render_Reflect();
		}
	}

	return S_OK;
}

void CSigrid::ImGui_RenderProperty()
{
	__super::ImGui_RenderProperty();

	//ImGui::InputInt("Coin", &m_tStatus.iCoin, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Navigation", (_int*)&m_eCurNavigation, 1, 1);

	_int	iCurNavigation = (_int)m_eCurNavigation;
	CGameUtility::Saturate(iCurNavigation, (_int)NAVI_END, 0);
	m_eCurNavigation = (NAVIGATIONTYPE)iCurNavigation;

	m_pSigridState->ImGui_RenderProperty();
}

_bool CSigrid::Collision_Range(CCollider * pTargetCollider)
{
	return m_pSphereCol->Collision(pTargetCollider);
}

_bool CSigrid::Collision_Body(CCollider * pTargetCollider)
{
	return m_pOBBCol->Collision(pTargetCollider);
}

_bool CSigrid::Collision_Net(CCollider * pTargetCollider)
{
	return m_pNetSphereCol->Collision(pTargetCollider);
}

void CSigrid::Collision_Event(CEnemy * pEnemy)
{
	if (m_tStatus.dCurHitCoolTime < m_tStatus.dInitHitCoolTime)
		m_bHit = false;
	else
	{
		m_bHit = true;
		m_tStatus.dCurHitCoolTime = 0.0;
	}

	if (m_bHit == true)
	{
		if (pEnemy != nullptr)
		{
			if (pEnemy->Is_SpecialAttack() == false)
				m_tStatus.iHP -= pEnemy->Get_Status().iAttack;
			else
				m_tStatus.iHP -= pEnemy->Get_Status().iSpecialAttack;
		}
		else
			m_tStatus.iHP -= 5;
	}

	if (m_tStatus.iHP < 0)
		m_tStatus.iHP = 0;
}

HRESULT CSigrid::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Sigrid", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_Stripes", L"Com_Outfit_Stripe", (CComponent**)&m_pOutfitTextureCom[0], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_FlowerPower", L"Com_Outfit_FlowerPower", (CComponent**)&m_pOutfitTextureCom[1], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_Camouflage", L"Com_Outfit_Camouflage", (CComponent**)&m_pOutfitTextureCom[2], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_Outfit_Hearts", L"Com_Outfit_Hearts", (CComponent**)&m_pOutfitTextureCom[3], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_HairMask", L"Com_HairMask", (CComponent**)&m_pHairMaskTextureCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Sigrid_NetRing", L"Com_NetRing", (CComponent**)&m_pNetRingTextureCom, this), E_FAIL);
	
	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("Sigrid_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.5f, Ymax - Ymin, (Zmax - Zmin) * 0.9f);
	ColliderDesc.vPosition = _float3(0.f, 0.9f, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(-90.f), 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_OBB", L"Com_OBB", (CComponent**)&m_pOBBCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("NetRing_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 4.f, (Ymax - Ymin) * 4.f, (Zmax - Zmin) * 4.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_NetSphere", (CComponent**)&m_pNetSphereCol, this, &ColliderDesc), E_FAIL);

	CNavigation::NAVIGATIONDESC		NavigationDesc;
	ZeroMemory(&NavigationDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NavigationDesc.iCurrentIndex = 25;

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_World", L"Com_Navigation_Default", (CComponent**)&m_pNavigationCom[NAVI_DEFAULT], this, &NavigationDesc), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Roof", L"Com_Navigation_Roof", (CComponent**)&m_pNavigationCom[NAVI_ROOF], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Sky", L"Com_Navigation_Sky", (CComponent**)&m_pNavigationCom[NAVI_SKY], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Fantasy_Island", L"Com_Navigation_Fantasy_Island", (CComponent**)&m_pNavigationCom[NAVI_FANTASY], this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Navigation_Hellheim", L"Com_Navigation_Hellheim", (CComponent**)&m_pNavigationCom[NAVI_HELLHEIM], this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CSigrid::SetUp_Parts_Hat()
{
	CGameObject*	pHat = nullptr;

	HATDESC	HatDesc;
	ZeroMemory(&HatDesc, sizeof(HatDesc));
	HatDesc.matSocketPivot = m_pModelCom->Get_PivotMatrix();
	HatDesc.pSocket = m_pModelCom->Get_BoneFromEntireBone("Sigrid DEF Nose");
	HatDesc.pPlayer = this;
	HatDesc.pPlayerTransformCom = m_pTransformCom;

	pHat = CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Hat_CrabbyHat", &HatDesc);
	NULL_CHECK_RETURN(pHat, E_FAIL);
	m_vecHats.push_back(pHat);

	pHat = CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Hat_FuzzyEars", &HatDesc);
	NULL_CHECK_RETURN(pHat, E_FAIL);
	m_vecHats.push_back(pHat);

	pHat = CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Hat_JellyCorne", &HatDesc);
	NULL_CHECK_RETURN(pHat, E_FAIL);
	m_vecHats.push_back(pHat);

	pHat = CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_Hat_ToothyHood", &HatDesc);
	NULL_CHECK_RETURN(pHat, E_FAIL);
	m_vecHats.push_back(pHat);

	return S_OK;
}

HRESULT CSigrid::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pHairMaskTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_MaskTexture");

	if (m_bHairMask)
	{
		if (m_iCurrentHair != 0)
			m_pShaderCom->Set_RawValue(L"g_HairColor", &m_vHairColor[m_iCurrentHair - 1], sizeof(_float4));
		if (m_iPreviewHair != 0)
			m_pShaderCom->Set_RawValue(L"g_HairColor", &m_vHairColor[m_iPreviewHair - 1], sizeof(_float4));
	}
	m_pShaderCom->Set_RawValue(L"g_WinSize", &_float2((_float)g_iWinSizeX, (_float)g_iWinSizeY),  sizeof(_float2));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSigrid::SetUp_ShaderResource_LightDepth()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	
	const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);

	//_float4	vLightEye = pLightDesc->vPosition;
	//_float4	vLightAt = XMLoadFloat4(&vLightEye) + XMVector3Normalize(XMLoadFloat4(&pLightDesc->vDirection));
	//_float4	vLightUp = { 0.f, 1.f, 0.f, 0.f };

	_float4		vLightEye = _float4(0.f, 300.f, 0.f, 1.f);
	_float4		vLightAt = _float4(900.f, 0.f, 900.f, 1.f);
	_float4		vLightUp = _float4(0.f, 1.f, 0.f, 0.f);

	_float4x4	matLightView = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4	matLightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)g_iWinSizeX / (_float)g_iWinSizeY, 0.1f, 1000.f);

	m_pShaderCom->Set_Matrix(L"g_matView", &matLightView);
	//m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_Matrix(L"g_matProj", &matLightProj);
	m_pShaderCom->Set_RawValue(L"g_WinSize", &_float2((_float)g_iWinSizeX, (_float)g_iWinSizeY), sizeof(_float2));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSigrid::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);

	_float4x4		matWorld = m_pTransformCom->Get_WorldMatrix();
	
	if (m_bOnOcean == true)
		matWorld._42 = 0.f;

	m_pShaderCom->Set_Matrix(L"g_matWorld", &matWorld);
	m_pShaderCom->Set_Matrix(L"g_matReflectView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pHairMaskTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_MaskTexture");

	if (m_bHairMask)
	{
		if (m_iCurrentHair != 0)
			m_pShaderCom->Set_RawValue(L"g_HairColor", &m_vHairColor[m_iCurrentHair - 1], sizeof(_float4));
		if (m_iPreviewHair != 0)
			m_pShaderCom->Set_RawValue(L"g_HairColor", &m_vHairColor[m_iPreviewHair - 1], sizeof(_float4));
	}
	m_pShaderCom->Set_RawValue(L"g_WinSize", &_float2((_float)g_iWinSizeX, (_float)g_iWinSizeY), sizeof(_float2));
	m_pShaderCom->Set_RawValue(L"g_vClipPlane", &pGameInstance->Get_ClipPlane(CPipeLine::CLIPPLANE_REFLECT), sizeof(_float4));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSigrid::SetOn_Terrain()
{
	_float4	vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	/*COcean*	pOcean = dynamic_cast<COcean*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Ocean")->front());
	CVIBuffer_Terrain*		pVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(pOcean->Get_Component(L"Com_VIBuffer"));
	_uint		iNumVerticesX = pVIBuffer->Get_NumVerticesX();
	_uint		iNumVerticesZ = pVIBuffer->Get_NumVerticesZ();
	_float3*	pTerrainPos = pVIBuffer->Get_TerrainPosition();

	_uint	iIndex = (_int)vPlayerPos.z * iNumVerticesX + (_int)vPlayerPos.x;

	if (iIndex < 0 || iIndex > (iNumVerticesX - 1) * (iNumVerticesZ - 1) || vPlayerPos.x < 0.f || vPlayerPos.z < 0.f)
		vPlayerPos.y = 0.f;
	else
	{
		_float	fWidth = vPlayerPos.x - pTerrainPos[iIndex + iNumVerticesX].x;
		_float	fHeight = pTerrainPos[iIndex + iNumVerticesX].z - vPlayerPos.z;

		_vector	vPlane;

		if (fWidth > fHeight)
			vPlane = XMPlaneFromPoints(pTerrainPos[iIndex + iNumVerticesX], pTerrainPos[iIndex + iNumVerticesX + 1], pTerrainPos[iIndex + 1]);
		else
			vPlane = XMPlaneFromPoints(pTerrainPos[iIndex + iNumVerticesX], pTerrainPos[iIndex + 1], pTerrainPos[iIndex]);

		vPlayerPos.y = (XMVectorGetX(vPlane) * vPlayerPos.x * -1.f - XMVectorGetZ(vPlane) * vPlayerPos.z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
	}*/

	vPlayerPos.y = 0.f;
	m_fGroundHeight = vPlayerPos.y;

	//if (m_bOnOcean == true && m_bJump == false)
	if (m_bJump == false)
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMLoadFloat4(&vPlayerPos));
}

void CSigrid::SetOn_Navigation()
{
	_float4	vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);

	vPlayerPos = m_pNavigationCom[m_eCurNavigation]->Get_CellHeight(vPlayerPos);
	m_fGroundHeight = vPlayerPos.y;

	if (m_bJump == false)
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMLoadFloat4(&vPlayerPos));
}

CSigrid::NAVIGATIONTYPE CSigrid::Check_CurrentNavigation()
{
	NAVIGATIONTYPE	eType = NAVI_END;
	_float					fMinDist = 10000.f;

	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	_int		iIndex = -1;

	for (_uint i = 0; i < (_uint)NAVI_END; ++i) 
	{
		if (m_pNavigationCom[i] != nullptr)
		{
			_float		fDist = m_pNavigationCom[i]->IsOnNavigation(vPos, iIndex);

			if (fDist != 0.f && fMinDist > fDist)
			{
				fMinDist = fDist;
				eType = (NAVIGATIONTYPE)i;
			}
		}
	}

	if (iIndex != -1 && eType != NAVI_END)
	{
		m_eCurNavigation = eType;
		m_pNavigationCom[m_eCurNavigation]->Set_CurrentCellIndex(iIndex);
	}

	return eType;
}

CSigrid * CSigrid::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CSigrid*		pInstance = new CSigrid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSigrid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSigrid::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CSigrid*		pInstance = new CSigrid(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CSigrid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSigrid::Free()
{
	__super::Free();

	for (auto& pGameObject : m_vecHats)
		Safe_Release(pGameObject);
	m_vecHats.clear();

	Safe_Release(m_pSigridState);

	for (_uint i = 0; i < (_uint)NAVI_END; ++i)
		Safe_Release(m_pNavigationCom[i]);

	for (_uint i = 0; i < 4; ++i)
		Safe_Release(m_pOutfitTextureCom[i]);

	Safe_Release(m_pNetRingTextureCom);
	Safe_Release(m_pHairMaskTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pOBBCol);
	Safe_Release(m_pNetSphereCol);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStateMachineCom);
}
