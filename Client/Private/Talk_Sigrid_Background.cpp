#include "stdafx.h"
#include "..\Public\Talk_Sigrid_Background.h"
#include "GameInstance.h"
#include "Talk_Button1.h"
#include "Talk_Button2.h"

CTalk_Sigrid_Background::CTalk_Sigrid_Background(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_Sigrid_Background::CTalk_Sigrid_Background(const CTalk_Sigrid_Background & rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_Sigrid_Background::Initialize_Prototype()
{
	m_wstrUITag = L"UI_Talk_Sigrid_BackGround";

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CTalk_Sigrid_Background::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX * 0.2f;
	m_fSizeY = (_float)g_iWinSizeY * 0.1f;
	m_fX = (_float)g_iWinSizeX * 0.18f;
	m_fY = (_float)g_iWinSizeY * -0.22f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	FAILED_CHECK_RETURN(SetUp_Parts(), E_FAIL);

	return S_OK;
}

void CTalk_Sigrid_Background::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_pButton1 != nullptr && m_pButton2 != nullptr && m_pButton1->Get_Sentence() == L"" && m_pButton2->Get_Sentence() == L"")
	{
		m_pButton1->Set_Sentence(m_wstrSentences[0]);
		m_pButton2->Set_Sentence(m_wstrSentences[1]);
	}

	if (g_bNeedResizeSwapChain)
	{
		m_fX = (_float)g_iWinSizeX * 0.18f;
		m_fY = (_float)g_iWinSizeY * -0.22f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
	}

	if (CGameInstance::GetInstance()->Key_Up(DIK_R))
	{
		for (auto& pUI : m_vecChild)
			pUI->Set_Dead();

		m_bDead = true;
	}

	for (auto& pUI : m_vecChild)
	{
		pUI->Tick(dTimeDelta);

		if (pUI->Is_Dead() == true)
		{
			m_bDead = true;
			break;
		}
	}
}

void CTalk_Sigrid_Background::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	for (auto& pUI : m_vecChild)
		pUI->Late_Tick(dTimeDelta);
}

HRESULT CTalk_Sigrid_Background::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTalk_Sigrid_Background::ImGui_RenderProperty()
{
}

HRESULT CTalk_Sigrid_Background::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_UI_Talk_Sigrid_BackGround", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CTalk_Sigrid_Background::SetUp_Parts()
{
	CUI*	pUI = nullptr;
	
	m_pButton1 = dynamic_cast<CTalk_Button1*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_Button1"));
	NULL_CHECK_RETURN(m_pButton1, E_FAIL);
	m_pButton1->Set_Parent(this);

	m_vecChild.push_back(m_pButton1);

	m_pButton2 = dynamic_cast<CTalk_Button2*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_Button2"));
	NULL_CHECK_RETURN(m_pButton2, E_FAIL);
	m_pButton2->Set_Parent(this);

	m_vecChild.push_back(m_pButton2);

	pUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject(L"Prototype_GameObject_UI_Talk_Sigrid_Icon"));
	NULL_CHECK_RETURN(pUI, E_FAIL);
	pUI->Set_Parent(this);

	m_vecChild.push_back(pUI);

	return S_OK;
}

HRESULT CTalk_Sigrid_Background::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CTalk_Sigrid_Background * CTalk_Sigrid_Background::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTalk_Sigrid_Background*	pInstance = new CTalk_Sigrid_Background(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTalk_Sigrid_Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk_Sigrid_Background::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTalk_Sigrid_Background*	pInstance = new CTalk_Sigrid_Background(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CTalk_Sigrid_Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalk_Sigrid_Background::Free()
{
	__super::Free();

	for (auto& pUI : m_vecChild)
		Safe_Release(pUI);
}
