#include "stdafx.h"
#include "..\Public\Shop_ArrowDown.h"
#include "GameInstance.h"

CShop_ArrowDown::CShop_ArrowDown(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_ArrowDown::CShop_ArrowDown(const CShop_ArrowDown & rhs)
	: CUI(rhs)
{
}

HRESULT CShop_ArrowDown::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Shop_ArrowDown";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_ArrowDown::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.015f;
	m_fSizeY = (_float)g_iWinSizeY * 0.045f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	return S_OK;
}

void CShop_ArrowDown::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = fParentSizeX * 0.45f;
		m_fY = -fParentSizeY * 0.27f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = fParentSizeX * 0.45f;
		m_fY = -fParentSizeY * 0.27f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}
}

void CShop_ArrowDown::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShop_ArrowDown::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShop_ArrowDown::ImGui_RenderProperty()
{
}

HRESULT CShop_ArrowDown::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ArrowDown", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_ArrowDown::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CShop_ArrowDown * CShop_ArrowDown::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_ArrowDown*	pInstance = new CShop_ArrowDown(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_ArrowDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_ArrowDown::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_ArrowDown*	pInstance = new CShop_ArrowDown(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_ArrowDown");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_ArrowDown::Free()
{
	__super::Free();
}
