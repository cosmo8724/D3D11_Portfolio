#include "stdafx.h"
#include "..\Public\Shop_Button2.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Cursor.h"
#include "Shop_BackGround.h"
#include "Static_Camera.h"

CShop_Button2::CShop_Button2(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CShop_Button2::CShop_Button2(const CShop_Button2 & rhs)
	: CUI(rhs)
{
}

HRESULT CShop_Button2::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Shop_Button2";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShop_Button2::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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

void CShop_Button2::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.277f;
		m_fY = -fParentSizeY * 0.445f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.277f;
		m_fY = -fParentSizeY * 0.445f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (CGameUtility::Rect_Picking(g_hWnd, m_Rect) && m_bReadyToDead == false)
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

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_2) || m_bMouseHover == true)
	{
		if (m_bMouseHover && CGameInstance::GetInstance()->Mouse_Down(DIM_LB))
			m_bReadyToDead = true;

		if (m_bReadyToDead == false)
		{
			if (m_fHoverScale < 1.3f)
				m_fHoverScale += (_float)dTimeDelta;
			else
				m_fHoverScale = 1.3f;
		}
	}
	else
	{
		if (m_fHoverScale > 1.f)
			m_fHoverScale -= (_float)dTimeDelta;
		else
			m_fHoverScale = 1.f;
	}

	if (CGameInstance::GetInstance()->Key_Up(DIK_2))
		m_bReadyToDead = true;
		
	if (m_bReadyToDead == true && m_fHoverScale == 1.f)
	{
		m_pParent->Set_Dead();
		Find_UI(LEVEL_TESTSTAGE, L"UI_Scene_Change_1")->Set_Dead();

		_bool	bReverse = true;
		CGameInstance::GetInstance()->Clone_GameObject(LEVEL_TESTSTAGE, L"Layer_UI", L"Prototype_GameObject_SceneChange_1", &bReverse);
		g_bReadySceneChange = false;

		if (CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->size())
			dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back())->Set_MouseFix(true);
	}

	m_pTransformCom->Set_Scale(_float3(m_fSizeX * m_fHoverScale, m_fSizeY * m_fHoverScale, 1.f));
}

void CShop_Button2::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

	m_Rect = { _long(((g_iWinSizeX * 0.5f) + vPos.x) - m_fSizeX * 0.5f), _long(((g_iWinSizeY * 0.5f) - vPos.y) - m_fSizeY * 0.5f),
		_long(((g_iWinSizeX * 0.5f) + vPos.x) + m_fSizeX * 0.5f), _long(((g_iWinSizeY * 0.5f) - vPos.y) + m_fSizeY * 0.5f) };

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShop_Button2::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

	CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrNo, _float2(vPos.x + 1325.f, -vPos.y + 720.f), 0.f, _float2(m_fAspectRatioX, m_fAspectRatioY));

	return S_OK;
}

void CShop_Button2::ImGui_RenderProperty()
{
}

HRESULT CShop_Button2::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Button2", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CShop_Button2::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CShop_Button2 * CShop_Button2::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CShop_Button2*	pInstance = new CShop_Button2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop_Button2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShop_Button2::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CShop_Button2*	pInstance = new CShop_Button2(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CShop_Button2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop_Button2::Free()
{
	__super::Free();
}