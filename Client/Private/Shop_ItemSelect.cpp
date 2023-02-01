#include "stdafx.h"
#include "..\Public\Shop_ItemSelect.h"
#include "GameInstance.h"

CShop_ItemSelect::CShop_ItemSelect(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_ItemSelect::CShop_ItemSelect(const CShop_ItemSelect & rhs)
	: CUI(rhs)
	, m_eFrame(rhs.m_eFrame)
{
}

HRESULT CShop_ItemSelect::Initialize_Prototype(FRAME eFrame)
{
	m_wstrUITag = L"UI_ItemSelect";
	m_eFrame = eFrame;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_ItemSelect::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.01f;
	m_fSizeY = (_float)g_iWinSizeY * 0.115f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	return S_OK;
}

void CShop_ItemSelect::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		if (m_eFrame == FRAME_LEFT)
			m_fX = fParentSizeX * -0.39f;
		else if (m_eFrame == FRAME_RIGHT)
			m_fX = fParentSizeX * 0.41f;

		m_fY = fParentSizeY * 0.242f - m_iCurItemSlot * 164.f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		if (m_eFrame == FRAME_LEFT)
			m_fX = fParentSizeX * -0.39f;
		else if (m_eFrame == FRAME_RIGHT)
			m_fX = fParentSizeX * 0.41f;

		m_fY = fParentSizeY * 0.242f - m_iCurItemSlot * 164.f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));
	m_fY = fParentSizeY * 0.242f - m_iCurItemSlot * 164.f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
}

void CShop_ItemSelect::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShop_ItemSelect::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShop_ItemSelect::ImGui_RenderProperty()
{
}

HRESULT CShop_ItemSelect::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);

	if (m_eFrame == FRAME_LEFT)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemSelectL", L"Com_Texture_L", (CComponent**)&m_pTextureCom, this), E_FAIL);
	}
	else if (m_eFrame == FRAME_RIGHT)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ItemSelectR", L"Com_Texture_R", (CComponent**)&m_pTextureCom, this), E_FAIL);
	}

	return S_OK;
}

HRESULT CShop_ItemSelect::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CShop_ItemSelect * CShop_ItemSelect::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, FRAME eFrame)
{
	CShop_ItemSelect*	pInstance = new CShop_ItemSelect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eFrame)))
	{
		MSG_BOX("Failed to Create : CShop_ItemSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_ItemSelect::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_ItemSelect*	pInstance = new CShop_ItemSelect(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_ItemSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_ItemSelect::Free()
{
	__super::Free();
}
