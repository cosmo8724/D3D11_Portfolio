#include "stdafx.h"
#include "..\Public\Shop_ItemBar.h"
#include "GameInstance.h"
#include "Shop_Menu_Cloth.h"
#include "Shop_Menu_Hair.h"
#include "Shop_Menu_Hat.h"

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
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &ItemBarDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

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

	if (m_bSelected)
	{
		if (m_fHoverScale < 1.06f)
			m_fHoverScale += (_float)dTimeDelta * 0.4f;
		else
			m_fHoverScale = 1.06f;
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

	m_wstrState = L"º¸À¯Áß";
	CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrItemName, _float2(vPos.x + 950.f, -vPos.y + 715.f), 0.f, _float2(m_fAspectRatioX * 1.3f, m_fAspectRatioY * 1.3f));
	CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrState, _float2(vPos.x + 1620.f, -vPos.y + 705.f), 0.f, _float2(m_fAspectRatioX * 1.8f, m_fAspectRatioY * 1.8f));

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
