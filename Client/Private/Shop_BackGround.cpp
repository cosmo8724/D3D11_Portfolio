#include "stdafx.h"
#include "..\Public\Shop_BackGround.h"
#include "GameInstance.h"
#include "Shop_Menu_Cloth.h"
#include "Shop_Menu_Hair.h"
#include "Shop_Menu_Hat.h"
#include "Shop_ItemSelect.h"
#include "Static_Camera.h"

CShop_BackGround::CShop_BackGround(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_BackGround::CShop_BackGround(const CShop_BackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CShop_BackGround::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Shop_BackGround";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_BackGround::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.6f;
	m_fSizeY = (_float)g_iWinSizeY * 0.8f;
	m_fX = (_float)g_iWinSizeX * 0.2f;
	m_fY = -(_float)g_iWinSizeY * 0.11f;
	m_iCurrentMenu = -1;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);
	
	//g_bShopOpen = true;

	return S_OK;
}

void CShop_BackGround::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (g_bNeedResizeSwapChain)
	{
		m_fX = (_float)g_iWinSizeX * 0.2f;
		m_fY = -(_float)g_iWinSizeY * 0.11f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_E))
		m_iCurrentMenu++;
	else if (CGameInstance::GetInstance()->Key_Down(DIK_Q))
		m_iCurrentMenu--;

	if (m_iCurrentMenu > 2)
		m_iCurrentMenu = 0;
	else if (m_iCurrentMenu < 0)
		m_iCurrentMenu = 2;

	if (m_iCurrentMenu == 0)
	{
		m_pMenuCloth->Set_Selected(true);
		m_pMenuHair->Set_Selected(false);
		m_pMenuHat->Set_Selected(false);
	}
	else if (m_iCurrentMenu == 1)
	{
		m_pMenuCloth->Set_Selected(false);
		m_pMenuHair->Set_Selected(true);
		m_pMenuHat->Set_Selected(false);
	}
	else if (m_iCurrentMenu == 2)
	{
		m_pMenuCloth->Set_Selected(false);
		m_pMenuHair->Set_Selected(false);
		m_pMenuHat->Set_Selected(true);
	}

	if (CGameInstance::GetInstance()->Key_Down(DIK_W))
		m_iCurrentSlot--;
	else if (CGameInstance::GetInstance()->Key_Down(DIK_S))
		m_iCurrentSlot++;

	if (m_iCurrentSlot > 4)
		m_iCurrentSlot = 0;
	else if (m_iCurrentSlot < 0)
		m_iCurrentSlot = 4;

	m_pItemSelectL->Set_ItemSlot(m_iCurrentSlot);
	m_pItemSelectR->Set_ItemSlot(m_iCurrentSlot);

	for (auto& pUI : m_vecChild)
		pUI->Tick(dTimeDelta);
}

void CShop_BackGround::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_bMouseHover = false;

	if (m_bDead == true)
		return;

	if (nullptr != m_pMenuCloth)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, m_pMenuCloth);

	if (nullptr != m_pMenuHair)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, m_pMenuHair);

	if (nullptr != m_pMenuHat)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, m_pMenuHat);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pUI : m_vecChild)
		pUI->Late_Tick(dTimeDelta);
}

HRESULT CShop_BackGround::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShop_BackGround::ImGui_RenderProperty()
{
	for (auto& pUI : m_vecChild)
		pUI->ImGui_RenderTransformCom();
}

HRESULT CShop_BackGround::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_BackGround", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_BackGround::SetUp_Parts()
{
	CUI*	pUI = nullptr;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_MonsterCounter"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_Button1"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_Button2"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ButtonA"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ButtonD"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ButtonW"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ButtonS"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ButtonQ"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ButtonE"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ArrowUp"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ArrowDown"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_TurnLeft"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_TurnRight"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	m_pMenuCloth = dynamic_cast<CShop_Menu_Cloth*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_Menu_Cloth"));
	NULL_CHECK_RETURN(m_pMenuCloth, E_FAIL);
	m_pMenuCloth->Set_Parent(this);

	m_vecChild.push_back(m_pMenuCloth);

	m_pMenuHair = dynamic_cast<CShop_Menu_Hair*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_Menu_Hair"));
	NULL_CHECK_RETURN(m_pMenuHair, E_FAIL);
	m_pMenuHair->Set_Parent(this);

	m_vecChild.push_back(m_pMenuHair);

	m_pMenuHat= dynamic_cast<CShop_Menu_Hat*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_Menu_Hat"));
	NULL_CHECK_RETURN(m_pMenuHat, E_FAIL);
	m_pMenuHat->Set_Parent(this);

	m_vecChild.push_back(m_pMenuHat);

	m_pItemSelectL = dynamic_cast<CShop_ItemSelect*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemSelectL"));
	NULL_CHECK_RETURN(m_pItemSelectL, E_FAIL);
	m_pItemSelectL->Set_Parent(this);

	m_vecChild.push_back(m_pItemSelectL);

	m_pItemSelectR = dynamic_cast<CShop_ItemSelect*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemSelectR"));
	NULL_CHECK_RETURN(m_pItemSelectR, E_FAIL);
	m_pItemSelectR->Set_Parent(this);

	m_vecChild.push_back(m_pItemSelectR);

	return S_OK;
}

HRESULT CShop_BackGround::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CShop_BackGround * CShop_BackGround::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_BackGround*	pInstance = new CShop_BackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_BackGround::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_BackGround*	pInstance = new CShop_BackGround(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_BackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_BackGround::Free()
{
	__super::Free();

	for (auto& pUI : m_vecChild)
		Safe_Release(pUI);

	g_bReadySceneChange = false;
	g_bShopOpen = false;
}
