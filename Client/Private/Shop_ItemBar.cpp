#include "stdafx.h"
#include "..\Public\Shop_ItemBar.h"
#include "GameInstance.h"
#include "Shop_Menu_Cloth.h"
#include "Shop_Menu_Hair.h"
#include "Shop_Menu_Hat.h"
#include "Sigrid.h"
#include "Shop_Button1.h"
#include "Shop_BackGround.h"

CShop_ItemBar::CShop_ItemBar(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_ItemBar::CShop_ItemBar(const CShop_ItemBar & rhs)
	: CUI(rhs)
	, m_wstrItemName(rhs.m_wstrItemName)
	, m_wstrState(rhs.m_wstrState)
{
}

HRESULT CShop_ItemBar::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_ItemBar::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.45f;
	m_fSizeY = (_float)g_iWinSizeY * 0.09f;
	m_fX = 0.f;
	m_fY = 0.f;

	ITEMBARDESC	ItemBarDesc;

	if (pArg != nullptr)
	{
		ItemBarDesc = *(ITEMBARDESC*)pArg;
		m_wstrUITag = ItemBarDesc.wstrUITag;
		m_wstrItemName = ItemBarDesc.wstrItemName;
		m_iSlot = ItemBarDesc.iSlot;
		m_iPrice = ItemBarDesc.iPrice;
		m_eType = ItemBarDesc.eType;

		if (m_eType == BLACK)
			m_vFontColor = { 0.4f, 0.96f, 0.04f, 1.f };
		else if (m_eType == ORANGE)
			m_vFontColor = { 0.3f, 1.f, 0.91f, 1.f };
		else if (m_eType == PINK)
			m_vFontColor = FLOAT4COLOR_RGBA(209.f, 30.f, 113.f, 255.f);
		else if (m_eType == WHITE)
			m_vFontColor = { 1.f, 1.f, 1.f, 1.f };
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &ItemBarDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

	m_pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	return S_OK;
}

void CShop_ItemBar::Tick(_double dTimeDelta)
{
	if (m_vecChild.empty())
		SetUp_Parts();

	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_Parent()->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_Parent()->Get_WorldMatrix()).r[1]));

		m_fX = fParentSizeX * 0.01f;
		m_fY = fParentSizeY * 0.24f - m_iSlot * (_float)g_iWinSizeY * 0.11f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_Parent()->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_Parent()->Get_WorldMatrix()).r[1]));

		m_fX = fParentSizeX * 0.01f;
		m_fY = fParentSizeY * 0.24f - m_iSlot * (_float)g_iWinSizeY * 0.11f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (dynamic_cast<CShop_Menu_Cloth*>(m_pParent))
		m_eState = (ITEMBAR_STATE)m_pPlayer->Get_ItemState(0, m_iSlot);
	else if (dynamic_cast<CShop_Menu_Hair*>(m_pParent))
		m_eState = (ITEMBAR_STATE)m_pPlayer->Get_ItemState(1, m_iSlot);
	else if (dynamic_cast<CShop_Menu_Hat*>(m_pParent))
		m_eState = (ITEMBAR_STATE)m_pPlayer->Get_ItemState(2, m_iSlot);

	if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED)
	{
		if (dynamic_cast<CShop_Menu_Cloth*>(m_pParent))
		{
			if (dynamic_cast<CShop_Menu_Cloth*>(m_pParent)->Get_CurEquipSlot() != m_iSlot)
			{
				m_eState = CShop_ItemBar::ITEMBAR_PURCHASED;
				m_pPlayer->Set_ItemState(m_iMenu, m_iSlot, (_uint)ITEMBAR_PURCHASED);
			}
		}
		else if (dynamic_cast<CShop_Menu_Hair*>(m_pParent))
		{
			if (dynamic_cast<CShop_Menu_Hair*>(m_pParent)->Get_CurEquipSlot() != m_iSlot)
			{
				m_eState = CShop_ItemBar::ITEMBAR_PURCHASED;
				m_pPlayer->Set_ItemState(m_iMenu, m_iSlot, (_uint)ITEMBAR_PURCHASED);
			}
		}
		else if (dynamic_cast<CShop_Menu_Hat*>(m_pParent))
		{
			if (dynamic_cast<CShop_Menu_Hat*>(m_pParent)->Get_CurEquipSlot() != m_iSlot)
			{
				m_eState = CShop_ItemBar::ITEMBAR_PURCHASED;
				m_pPlayer->Set_ItemState(m_iMenu, m_iSlot, (_uint)ITEMBAR_PURCHASED);
			}
		}
	}

	if (m_bSelected)
	{
		if (m_fHoverScale < 1.06f)
			m_fHoverScale += (_float)dTimeDelta * 0.4f;
		else
			m_fHoverScale = 1.06f;

		CSigrid*	pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());

		CShop_Button1*	pUI = dynamic_cast<CShop_Button1*>(dynamic_cast<CShop_BackGround*>(m_pParent->Get_Parent())->Get_Child(L"UI_Shop_Button1"));
		NULL_CHECK_RETURN(pUI, );

		if (dynamic_cast<CShop_Menu_Cloth*>(m_pParent))
		{
			CShop_Menu_Cloth*		pMenu = dynamic_cast<CShop_Menu_Cloth*>(m_pParent);
			if (pMenu->Is_Selected() == true)
			{
				m_iMenu = 0;
				m_eState = (ITEMBAR_STATE)pPlayer->Get_ItemState(m_iMenu, m_iSlot);

				pPlayer->Set_PreviewOutfit(m_iSlot);
				pPlayer->Set_PreviewHair(0);
				pPlayer->Set_PreviewHat(0);

				if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED || m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
					pUI->Set_ButtonText(L"����");
				else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
					pUI->Set_ButtonText(L"����");
			}
		}
		else if (dynamic_cast<CShop_Menu_Hair*>(m_pParent))
		{
			CShop_Menu_Hair*		pMenu = dynamic_cast<CShop_Menu_Hair*>(m_pParent);
			if (pMenu->Is_Selected() == true)
			{
				m_iMenu = 1;
				m_eState = (ITEMBAR_STATE)pPlayer->Get_ItemState(m_iMenu, m_iSlot);

				pPlayer->Set_PreviewOutfit(0);
				pPlayer->Set_PreviewHair(m_iSlot);
				pPlayer->Set_PreviewHat(0);

				if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED || m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
					pUI->Set_ButtonText(L"����");
				else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
					pUI->Set_ButtonText(L"����");
			}
		}
		else if (dynamic_cast<CShop_Menu_Hat*>(m_pParent))
		{
			CShop_Menu_Hat*		pMenu = dynamic_cast<CShop_Menu_Hat*>(m_pParent);
			if (pMenu->Is_Selected() == true)
			{
				m_iMenu = 2;
				m_eState = (ITEMBAR_STATE)pPlayer->Get_ItemState(m_iMenu, m_iSlot);

				pPlayer->Set_PreviewOutfit(0);
				pPlayer->Set_PreviewHair(0);
				pPlayer->Set_PreviewHat(m_iSlot);

				if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED || m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
					pUI->Set_ButtonText(L"����");
				else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
					pUI->Set_ButtonText(L"����");
			}
		}
	}
	else
	{
		if (m_fHoverScale > 1.f)
			m_fHoverScale -= (_float)dTimeDelta * 0.4f;
		else
			m_fHoverScale = 1.f;
	}

	m_pTransformCom->Set_Scale(_float3(m_fSizeX * m_fHoverScale, m_fSizeY * m_fHoverScale, 1.f));

	for (auto& pUI : m_vecChild)
		pUI->Tick(dTimeDelta);
}

void CShop_ItemBar::Late_Tick(_double dTimeDelta)
{
	//__super::Late_Tick(dTimeDelta);

	if (m_pParent != nullptr)
	{
		_matrix	matWorld, matParentWorld;
		matWorld = m_pTransformCom->Get_WorldMatrix();
		matParentWorld = m_pParent->Get_Parent()->Get_WorldMatrix();

		memcpy(&matParentWorld.r[0], &XMVector3Normalize(matParentWorld.r[0]), sizeof(_vector));
		memcpy(&matParentWorld.r[1], &XMVector3Normalize(matParentWorld.r[1]), sizeof(_vector));
		memcpy(&matParentWorld.r[2], &XMVector3Normalize(matParentWorld.r[2]), sizeof(_vector));

		m_matWorldmulParent = matWorld * matParentWorld;
	}

	if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED)
		m_wstrState = L"������";
	else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
		m_wstrState = to_wstring(m_iPrice);
	else if (m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
		m_wstrState = L"���� ��";

	if (CGameInstance::GetInstance()->Key_Down(DIK_1))
	{
		if (m_bSelected == true)
		{
			if (m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
			{
				m_eState = CShop_ItemBar::ITEMBAR_EQUIPED;
				m_pPlayer->Set_ItemState(m_iMenu, m_iSlot, (_uint)ITEMBAR_EQUIPED);
				m_pPlayer->EquipItem(m_iMenu, m_iSlot);

				if (dynamic_cast<CShop_Menu_Cloth*>(m_pParent))
					dynamic_cast<CShop_Menu_Cloth*>(m_pParent)->Set_CurEquipSlot(m_iSlot);
				else if (dynamic_cast<CShop_Menu_Hair*>(m_pParent))
					dynamic_cast<CShop_Menu_Hair*>(m_pParent)->Set_CurEquipSlot(m_iSlot);
				else if (dynamic_cast<CShop_Menu_Hat*>(m_pParent))
					dynamic_cast<CShop_Menu_Hat*>(m_pParent)->Set_CurEquipSlot(m_iSlot);
			}
			else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
			{
				_uint	iMoney = 0;

				if (m_eType == BLACK)
					iMoney = m_pPlayer->Get_Status().iMonsterDrink_Black;
				else if (m_eType == ORANGE)
					iMoney = m_pPlayer->Get_Status().iMonsterDrink_Orange;
				else if (m_eType == PINK)
					iMoney = m_pPlayer->Get_Status().iMonsterDrink_Pink;
				else if (m_eType == WHITE)
					iMoney = m_pPlayer->Get_Status().iMonsterDrink_White;

				if (iMoney >= m_iPrice)
				{
					m_eState = CShop_ItemBar::ITEMBAR_PURCHASED;
					m_pPlayer->Set_ItemState(m_iMenu, m_iSlot, (_uint)ITEMBAR_PURCHASED);
					m_pPlayer->Set_ItemPurchased(true);
					CGameInstance::GetInstance()->Play_Sound(L"SFX_UI_BaineShop_PurchaseSuccessful.wav", 1.f, false, true);
				}
				else
					CGameInstance::GetInstance()->Play_Sound(L"SFX_UI_MainMenu_SelectionChosen.wav", 1.f, false, true);
			}
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pUI : m_vecChild)
		pUI->Late_Tick(dTimeDelta);
}

HRESULT CShop_ItemBar::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

	CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrItemName, _float2(vPos.x + 950.f, -vPos.y + 715.f), 0.f, _float2(m_fAspectRatioX * 1.3f, m_fAspectRatioY * 1.3f));

	if (m_bSelected)
	{
		if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED)
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1610.f + (27.f * m_fHoverScale), -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f));
		else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1690.f + (33.f * m_fHoverScale), -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f), m_vFontColor);
		else if (m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1610.f + (27.f * m_fHoverScale), -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f));
	}
	else
	{
		if (m_eState == CShop_ItemBar::ITEMBAR_EQUIPED)
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1610.f, -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f));
		else if (m_eState == CShop_ItemBar::ITEMBAR_NOTPURCHASED)
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1700.f, -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f), m_vFontColor);
		else if (m_eState == CShop_ItemBar::ITEMBAR_PURCHASED)
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1610.f, -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f));
	}

	return S_OK;
}

void CShop_ItemBar::ImGui_RenderProperty()
{
}

HRESULT CShop_ItemBar::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemBar", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_ItemBar::SetUp_Parts()
{
	CUI*	pUI = nullptr;

	if (dynamic_cast<CShop_Menu_Cloth*>(m_pParent))
	{
		pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar_Icon_Cloth"));
		NULL_CHECK_RETURN(pUI, E_FAIL);
		pUI->Set_Parent(this);

		m_vecChild.push_back(pUI);
	}
	else if (dynamic_cast<CShop_Menu_Hair*>(m_pParent))
	{
		pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar_Icon_Hair"));
		NULL_CHECK_RETURN(pUI, E_FAIL);
		pUI->Set_Parent(this);

		m_vecChild.push_back(pUI);
	}
	else if (dynamic_cast<CShop_Menu_Hat*>(m_pParent))
	{
		pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar_Icon_Hat"));
		NULL_CHECK_RETURN(pUI, E_FAIL);
		pUI->Set_Parent(this);

		m_vecChild.push_back(pUI);
	}

	return S_OK;
}

HRESULT CShop_ItemBar::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture", (_uint)m_eState);

	return S_OK;
}

CShop_ItemBar * CShop_ItemBar::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_ItemBar*		pInstance = new CShop_ItemBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_ItemBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_ItemBar::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_ItemBar*		pInstance = new CShop_ItemBar(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_ItemBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_ItemBar::Free()
{
	__super::Free();

	for (auto& pUI : m_vecChild)
		Safe_Release(pUI);
}
