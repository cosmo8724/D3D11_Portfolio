#include "stdafx.h"
#include "..\Public\Shop_Menu_Hair.h"
#include "GameInstance.h"
#include "Shop_ItemBar.h"
#include "Shop_BackGround.h"
#include "Sigrid.h"

CShop_Menu_Hair::CShop_Menu_Hair(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_Menu_Hair::CShop_Menu_Hair(const CShop_Menu_Hair & rhs)
	: CUI(rhs)
{
}

HRESULT CShop_Menu_Hair::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Shop_Menu_Hair";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_Menu_Hair::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.067f;
	m_fSizeY = (_float)g_iWinSizeY * 0.08f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

	m_pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_iCurEquipSlot = m_pPlayer->Get_EquipSlot(1);

	return S_OK;
}

void CShop_Menu_Hair::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.285f;
		m_fY = fParentSizeY * 0.4f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.285f;
		m_fY = fParentSizeY * 0.4f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (m_bSelected == true)
	{
		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY * 1.4f, 1.f));

		_int	iCurrentSlot = dynamic_cast<CShop_BackGround*>(m_pParent)->Get_CurrentSlot();

		for (size_t i = 0; i < m_vecChild.size(); ++i)
		{
			if ((_int)i == iCurrentSlot)
				dynamic_cast<CShop_ItemBar*>(m_vecChild[i])->Set_Selected(true);
			else
				dynamic_cast<CShop_ItemBar*>(m_vecChild[i])->Set_Selected(false);
		}
	}
	else
		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));

	for (auto& pUI : m_vecChild)
		pUI->Tick(dTimeDelta);
}

void CShop_Menu_Hair::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_bSelected == true)
	{
		for (auto& pUI : m_vecChild)
			pUI->Late_Tick(dTimeDelta);
	}
}

HRESULT CShop_Menu_Hair::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShop_Menu_Hair::ImGui_RenderProperty()
{
}

HRESULT CShop_Menu_Hair::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Menu_Hair", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_Menu_Hair::SetUp_Parts()
{
	CUI*	pUI = nullptr;
	CShop_ItemBar::ITEMBARDESC		ItemBarDesc;

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Hair_1";
	ItemBarDesc.wstrItemName = L"¿ø·¡ ¸Ó¸®Ä«¶ô »ö±ò";
	ItemBarDesc.iSlot = 0;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Hair_2";
	ItemBarDesc.wstrItemName = L"°ñµç ·Ï ¿°»ö ¸Ó¸®";
	ItemBarDesc.iSlot = 1;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Hair_3";
	ItemBarDesc.wstrItemName = L"ÇÏ¾á Á¶°¡ºñ ¿°»ö ¸Ó¸®";
	ItemBarDesc.iSlot = 2;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Hair_4";
	ItemBarDesc.wstrItemName = L"ÆÇÅ¸½ºÆ½ ÄÚ¶ö ¿°»ö ¸Ó¸®";
	ItemBarDesc.iSlot = 3;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Hair_5";
	ItemBarDesc.wstrItemName = L"¿À´Ð½º ºí·¢ ¿°»ö ¸Ó¸®";
	ItemBarDesc.iSlot = 4;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	return S_OK;
}

HRESULT CShop_Menu_Hair::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture", (_uint)m_bSelected);

	return S_OK;
}

CShop_Menu_Hair * CShop_Menu_Hair::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_Menu_Hair*	pInstance = new CShop_Menu_Hair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_Menu_Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_Menu_Hair::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_Menu_Hair*	pInstance = new CShop_Menu_Hair(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_Menu_Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_Menu_Hair::Free()
{
	__super::Free();

	for (auto& pUI : m_vecChild)
		Safe_Release(pUI);
}
