#include "stdafx.h"
#include "..\Public\Shop_ButtonE.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Cursor.h"
#include "Shop_BackGround.h"

CShop_ButtonE::CShop_ButtonE(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_ButtonE::CShop_ButtonE(const CShop_ButtonE & rhs)
	: CUI(rhs)
{
}

HRESULT CShop_ButtonE::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Shop_ButtonE";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_ButtonE::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.028f;
	m_fSizeY = (_float)g_iWinSizeY * 0.045f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	return S_OK;
}

void CShop_ButtonE::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.1f;
		m_fY = fParentSizeY * 0.395f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.1f;
		m_fY = fParentSizeY * 0.395f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (CGameUtility::Rect_Picking(g_hWnd, m_Rect))
	{
		m_bMouseHover = true;
		dynamic_cast<CShop_BackGround*>(m_pParent)->Set_MouseHover();
		g_pCursor->Set_CursorState(CCursor::CURSOR_SELECT);
	}
	else
	{
		m_bMouseHover = false;

		if (dynamic_cast<CShop_BackGround*>(m_pParent)->Get_MouseHover() == false)
			g_pCursor->Set_CursorState(CCursor::CURSOR_DEFAULT);
	}

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_E) || m_bMouseHover)
	{
		if (m_bMouseHover && CGameInstance::GetInstance()->Mouse_Down(DIM_LB))
		{
			m_fHoverScale -= 0.15f;
			dynamic_cast<CShop_BackGround*>(m_pParent)->Get_CurrentMenu()++;
		}

		if (m_fHoverScale < 1.3f)
			m_fHoverScale += (_float)dTimeDelta;
		else
			m_fHoverScale = 1.3f;
	}
	else
	{
		if (m_fHoverScale > 1.f)
			m_fHoverScale -= (_float)dTimeDelta;
		else
			m_fHoverScale = 1.f;
	}

	m_pTransformCom->Set_Scale(_float3(m_fSizeX * m_fHoverScale, m_fSizeY * m_fHoverScale, 1.f));
}

void CShop_ButtonE::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

	m_Rect = { _long(((g_iWinSizeX * 0.5f) + vPos.x) - m_fSizeX * 0.5f), _long(((g_iWinSizeY * 0.5f) - vPos.y) - m_fSizeY * 0.5f),
		_long(((g_iWinSizeX * 0.5f) + vPos.x) + m_fSizeX * 0.5f), _long(((g_iWinSizeY * 0.5f) - vPos.y) + m_fSizeY * 0.5f) };

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShop_ButtonE::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CShop_ButtonE::ImGui_RenderProperty()
{
}

HRESULT CShop_ButtonE::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_ButtonE", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_ButtonE::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CShop_ButtonE * CShop_ButtonE::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_ButtonE*	pInstance = new CShop_ButtonE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_ButtonE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_ButtonE::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_ButtonE*	pInstance = new CShop_ButtonE(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_ButtonE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_ButtonE::Free()
{
	__super::Free();
}
