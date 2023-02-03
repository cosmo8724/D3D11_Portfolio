#include "stdafx.h"
#include "..\Public\Shop_Menu_Cloth.h"
#include "GameInstance.h"
#include "Shop_ItemBar.h"
#include "Shop_BackGround.h"
#include "Sigrid.h"

CShop_Menu_Cloth::CShop_Menu_Cloth(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_Menu_Cloth::CShop_Menu_Cloth(const CShop_Menu_Cloth & rhs)
	: CUI(rhs)
{
}

HRESULT CShop_Menu_Cloth::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Shop_Menu_Cloth";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_Menu_Cloth::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.065f;
	m_fSizeY = (_float)g_iWinSizeY * 0.1f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

	m_pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	m_iCurEquipSlot = m_pPlayer->Get_EquipSlot(0);

	return S_OK;
}

void CShop_Menu_Cloth::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.37f;
		m_fY = fParentSizeY * 0.385f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.37f;
		m_fY = fParentSizeY * 0.385f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (m_bSelected == true)
	{
		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY * 1.2f, 1.f));

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

void CShop_Menu_Cloth::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_bSelected == true)
	{
		for (auto& pUI : m_vecChild)
			pUI->Late_Tick(dTimeDelta);
	}
}

HRESULT CShop_Menu_Cloth::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShop_Menu_Cloth::ImGui_RenderProperty()
{
}

HRESULT CShop_Menu_Cloth::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Menu_Cloth", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_Menu_Cloth::SetUp_Parts()
{
	CUI*	pUI = nullptr;
	CShop_ItemBar::ITEMBARDESC		ItemBarDesc;

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Cloth_1";
	ItemBarDesc.wstrItemName = L"클래식 의상";
	ItemBarDesc.iSlot = 0;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Cloth_2";
	ItemBarDesc.wstrItemName = L"줄무늬";
	ItemBarDesc.iSlot = 1;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Cloth_3";
	ItemBarDesc.wstrItemName = L"플라워 파워";
	ItemBarDesc.iSlot = 2;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Cloth_4";
	ItemBarDesc.wstrItemName = L"위장";
	ItemBarDesc.iSlot = 3;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	ItemBarDesc.wstrUITag = L"UI_ItemBar_Cloth_5";
	ItemBarDesc.wstrItemName = L"하트 무늬";
	ItemBarDesc.iSlot = 4;

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Shop_ItemBar", &ItemBarDesc));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	return S_OK;
}

HRESULT CShop_Menu_Cloth::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture", (_uint)m_bSelected);

	return S_OK;
}

CShop_Menu_Cloth * CShop_Menu_Cloth::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_Menu_Cloth*		pInstance = new CShop_Menu_Cloth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_Menu_Cloth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_Menu_Cloth::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_Menu_Cloth*		pInstance = new CShop_Menu_Cloth(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_Menu_Cloth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_Menu_Cloth::Free()
{
	__super::Free();

	for (auto& pUI : m_vecChild)
		Safe_Release(pUI);
}
