#include "stdafx.h"
#include "..\Public\Talk_Button1.h"
#include "GameInstance.h"
#include "Talk_NPC_Background.h"

CTalk_Button1::CTalk_Button1(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_Button1::CTalk_Button1(const CTalk_Button1 & rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_Button1::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Talk_Button1";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTalk_Button1::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.0196f;
	m_fSizeY = (_float)g_iWinSizeY * 0.0315f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	return S_OK;
}

void CTalk_Button1::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.4f;
		m_fY = fParentSizeY * 0.2f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.4f;
		m_fY = fParentSizeY * 0.2f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (CGameInstance::GetInstance()->Key_Pressing(DIK_1))
	{
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

	if (CGameInstance::GetInstance()->Key_Up(DIK_1))
	{
		CTalk_NPC_Background* pUI = dynamic_cast<CTalk_NPC_Background*>(Find_UI(LEVEL_TESTSTAGE, L"UI_Talk_NPC_BackGround"));
		pUI->Set_Answer(1);

		m_bDead = true;
	}

	m_pTransformCom->Set_Scale(_float3(m_fSizeX * m_fHoverScale, m_fSizeY * m_fHoverScale, 1.f));
}

void CTalk_Button1::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

	m_Rect = { _long(((g_iWinSizeX * 0.5f) + vPos.x) - m_fSizeX * 0.5f), _long(((g_iWinSizeY * 0.5f) - vPos.y) - m_fSizeY * 0.5f),
		_long(((g_iWinSizeX * 0.5f) + vPos.x) + m_fSizeX * 0.5f), _long(((g_iWinSizeY * 0.5f) - vPos.y) + m_fSizeY * 0.5f) };

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTalk_Button1::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

	CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrSentence, _float2(vPos.x + 1310.f, -vPos.y + 726.f), 0.f, _float2(m_fAspectRatioX * 0.8f, m_fAspectRatioY * 0.8f));

	return S_OK;
}

void CTalk_Button1::ImGui_RenderProperty()
{
}

HRESULT CTalk_Button1::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Shop_Button1", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CTalk_Button1::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CTalk_Button1 * CTalk_Button1::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTalk_Button1*	pInstance = new CTalk_Button1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTalk_Button1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk_Button1::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTalk_Button1*	pInstance = new CTalk_Button1(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CTalk_Button1");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalk_Button1::Free()
{
	__super::Free();
}
