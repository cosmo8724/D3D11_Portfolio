#include "stdafx.h"
#include "..\Public\Talk_NPC_NameBox.h"
#include "GameInstance.h"

CTalk_NPC_NameBox::CTalk_NPC_NameBox(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_NPC_NameBox::CTalk_NPC_NameBox(const CTalk_NPC_NameBox & rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_NPC_NameBox::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Talk_NPC_NameBox";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTalk_NPC_NameBox::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.1f;
	m_fSizeY = (_float)g_iWinSizeY * 0.06f;
	m_fX = 0.f;
	m_fY = 0.f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

	return S_OK;
}

void CTalk_NPC_NameBox::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bFixPosition == false)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.28f;
		m_fY = fParentSizeY * 0.5f;

		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

		m_bFixPosition = true;
	}

	if (g_bNeedResizeSwapChain)
	{
		_float fParentSizeX = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[0]));
		_float fParentSizeY = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_pParent->Get_WorldMatrix()).r[1]));

		m_fX = -fParentSizeX * 0.28f;
		m_fY = fParentSizeY * 0.5f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}
}

void CTalk_NPC_NameBox::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTalk_NPC_NameBox::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	if (m_wstrName != L"")
	{
		_float4	vPos = XMLoadFloat4x4(&m_matWorldmulParent).r[3];

		if (m_wstrName == L"코델리아 클라우트")
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrName, _float2(vPos.x + 1175.f, -vPos.y + 730.f), 0.f, _float2(m_fAspectRatioX*0.7f, m_fAspectRatioY*0.7f));
		else if (m_wstrName == L"클레멘 클라우트")
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrName, _float2(vPos.x + 1170.f, -vPos.y + 725.f), 0.f, _float2(m_fAspectRatioX*0.8f, m_fAspectRatioY*0.8f));
		else
			CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrName, _float2(vPos.x + 1175.f, -vPos.y + 720.f), 0.f, _float2(m_fAspectRatioX, m_fAspectRatioY));
	}

	return S_OK;
}

void CTalk_NPC_NameBox::ImGui_RenderProperty()
{
}

HRESULT CTalk_NPC_NameBox::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_NPC_NameBox", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CTalk_NPC_NameBox::SetUp_Parts()
{
	return S_OK;
}

HRESULT CTalk_NPC_NameBox::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CTalk_NPC_NameBox * CTalk_NPC_NameBox::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTalk_NPC_NameBox*	pInstance = new CTalk_NPC_NameBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTalk_NPC_NameBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk_NPC_NameBox::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTalk_NPC_NameBox*	pInstance = new CTalk_NPC_NameBox(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CTalk_NPC_NameBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalk_NPC_NameBox::Free()
{
	__super::Free();
}
