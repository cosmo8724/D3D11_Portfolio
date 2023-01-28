#include "stdafx.h"
#include "..\Public\MonsterDrink_Icon.h"
#include "GameInstance.h"

CMonsterDrink_Icon::CMonsterDrink_Icon(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CMonsterDrink_Icon::CMonsterDrink_Icon(const CMonsterDrink_Icon & rhs)
	: CUI(rhs)
	, m_eColor(rhs.m_eColor)
{
}

HRESULT CMonsterDrink_Icon::Initialize_Prototype(CMonsterDrink_Frame::MONSTERCOLOR eColor)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	m_eColor = eColor;

	return S_OK;
}

HRESULT CMonsterDrink_Icon::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = 277.f;
	m_fSizeY = 380.f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(m_eColor), E_FAIL);

	if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_BLACK)
		m_pParent = Find_UI(LEVEL_TESTSTAGE, L"UI_MonsterDrink_Black");
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_ORANGE)
		m_pParent = Find_UI(LEVEL_TESTSTAGE, L"UI_MonsterDrink_Orange");
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_PINK)
		m_pParent = Find_UI(LEVEL_TESTSTAGE, L"UI_MonsterDrink_Pink");
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_WHITE)
		m_pParent = Find_UI(LEVEL_TESTSTAGE, L"UI_MonsterDrink_White");

	NULL_CHECK_RETURN(m_pParent, E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fX * 0.2f, m_fY * 0.2f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(-1.f, 17.f, 0.f, 1.f));

	return S_OK;
}

void CMonsterDrink_Icon::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (g_bNeedResizeSwapChain)
	{
		m_pTransformCom->Set_Scale(_float3(m_fX * 0.2f, m_fY * 0.2f, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(-1.f, 17.f, 0.f, 1.f));
	}
}

void CMonsterDrink_Icon::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMonsterDrink_Icon::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CMonsterDrink_Icon::SetUp_Component(CMonsterDrink_Frame::MONSTERCOLOR eColor)
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);

	if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_BLACK)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_Black", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);
	}
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_ORANGE)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_Orange", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);
	}
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_PINK)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_Pink", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);
	}
	else if (m_eColor == CMonsterDrink_Frame::MONSTERDRINK_WHITE)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_MonsterDrink_Icon_White", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);
	}

	return S_OK;
}

HRESULT CMonsterDrink_Icon::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	//m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matWorld", &m_matWorldmulParent);
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CMonsterDrink_Icon * CMonsterDrink_Icon::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CMonsterDrink_Frame::MONSTERCOLOR eColor)
{
	CMonsterDrink_Icon*	pInstance = new CMonsterDrink_Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColor)))
	{
		MSG_BOX("Failed to Create : CMonsterDrink_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterDrink_Icon::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CMonsterDrink_Icon*	pInstance = new CMonsterDrink_Icon(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CMonsterDrink_Icon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterDrink_Icon::Free()
{
	__super::Free();
}
