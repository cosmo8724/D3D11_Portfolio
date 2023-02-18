#include "stdafx.h"
#include "..\Public\HPBar.h"
#include "GameInstance.h"
#include "Sigrid.h"

CHPBar::CHPBar(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CHPBar::CHPBar(const CHPBar & rhs)
	: CUI(rhs)
{
}

HRESULT CHPBar::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CHPBar::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_wstrUITag = L"HPBar";
	m_fSizeX = 1842.f;
	m_fSizeY = 744.f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fX * 0.2f, m_fY * 0.2f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(-(_float)g_iWinSizeX * 0.43f, -(_float)g_iWinSizeY * 0.42f, 0.f, 1.f));

	m_pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());
	NULL_CHECK_RETURN(m_pPlayer, E_FAIL);

	return S_OK;
}

void CHPBar::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (g_bNeedResizeSwapChain)
	{
		m_pTransformCom->Set_Scale(_float3(m_fX * 0.2f, m_fY * 0.2f, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(-(_float)g_iWinSizeX * 0.43f, -(_float)g_iWinSizeY * 0.42f, 0.f, 1.f));
	}
}

void CHPBar::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (g_bEnd == true)
		return;

	m_wstrHP = to_wstring(m_pPlayer->Get_Status().iHP);

	if (nullptr != m_pRendererCom && g_bReadySceneChange == false && g_bShopOpen == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHPBar::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	CGameInstance::GetInstance()->Render_Font(L"Font_DoongSil", m_wstrHP, _float2(g_iWinSizeX * 0.07f, g_iWinSizeY * 0.908f), 0.f, _float2(m_fAspectRatioX, m_fAspectRatioY));

	return S_OK;
}

HRESULT CHPBar::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_HPBar", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CHPBar::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	return S_OK;
}

CHPBar * CHPBar::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CHPBar*	pInstance = new CHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHPBar::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CHPBar*	pInstance = new CHPBar(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHPBar::Free()
{
	__super::Free();
}
