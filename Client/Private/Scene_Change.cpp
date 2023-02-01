#include "stdafx.h"
#include "..\Public\Scene_Change.h"
#include "GameInstance.h"
#include "Static_Camera.h"

CScene_Change::CScene_Change(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CScene_Change::CScene_Change(const CScene_Change & rhs)
	: CUI(rhs)
	, m_iSpriteIndex(rhs.m_iSpriteIndex)
	, m_bReversePlay(rhs.m_bReversePlay)
	, m_iWidthFrame(rhs.m_iWidthFrame)
	, m_iHeightFrame(rhs.m_iHeightFrame)
	, m_iWidthCount(rhs.m_iWidthCount)
	, m_iHeightCount(rhs.m_iHeightCount)
{
}

HRESULT CScene_Change::Initialize_Prototype(_uint iSpriteIndex, _bool bReversePlay)
{
	m_iSpriteIndex = iSpriteIndex;
	m_bReversePlay = bReversePlay;

	if (m_iSpriteIndex == 0)
	{
		m_wstrUITag = L"UI_Scene_Change_0";
		m_iWidthCount = 4;
		m_iHeightCount = 8;
	}
	else if (m_iSpriteIndex == 1)
	{
		m_wstrUITag = L"UI_Scene_Change_1";
		m_iWidthCount = 6;
		m_iHeightCount = 9;
	}
	else if (m_iSpriteIndex == 2)
	{
		m_wstrUITag = L"UI_Scene_Change_2";
		m_iWidthCount = 4;
		m_iHeightCount = 8;
	}
	else if (m_iSpriteIndex == 3)
	{
		m_wstrUITag = L"UI_Scene_Change_3";
		m_iWidthCount = 3;
		m_iHeightCount = 8;
	}

	return S_OK;
}

HRESULT CScene_Change::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	if (pArg != nullptr)
		m_bColorReverse = *(_bool*)pArg;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back())->Set_MouseFix(false);

	return S_OK;
}

void CScene_Change::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (g_bNeedResizeSwapChain)
	{
		m_fX = m_fSizeX * 0.5f;
		m_fY = m_fSizeY * 0.5f;

		m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
	}

	if (m_bDead == true)
		return;

	m_dTimeDelta += dTimeDelta;

	if (m_bFinish == false && m_dTimeDelta > m_dSpeed)
	{
		m_dTimeDelta -= m_dSpeed;
		m_iWidthFrame++;

		if (m_iWidthFrame >= m_iWidthCount)
		{
			m_iHeightFrame++;

			if (m_iHeightFrame >= m_iHeightCount)
			{
				m_iHeightFrame--;
				m_bFinish = true;

				if (m_bColorReverse == false)
					g_bShopOpen = true;
				else
				{
					g_bReadySceneChange = false;
					m_bDead = true;
				}
			}
			else
				m_iWidthFrame = 0;
		}
	}
}

void CScene_Change::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this, true);
}

HRESULT CScene_Change::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResources(), E_FAIL);

	if (m_bColorReverse == false)
		m_pShaderCom->Begin(7);
	else
		m_pShaderCom->Begin(8);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CScene_Change::ImGui_RenderProperty()
{
	ImGui::InputDouble("Speed", &m_dSpeed, 0.01, 0.1);
}

HRESULT CScene_Change::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Texture_SceneChange", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CScene_Change::SetUp_ShaderResources()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");
	m_pShaderCom->Set_Matrix(L"g_matView", &m_matView);
	m_pShaderCom->Set_Matrix(L"g_matProj", &m_matProj);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture", m_iSpriteIndex);
	m_pShaderCom->Set_RawValue(L"g_WidthFrame", &m_iWidthFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightFrame", &m_iHeightFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_WidthCount", &m_iWidthCount, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightCount", &m_iHeightCount, sizeof(_uint));

	return S_OK;
}

CScene_Change * CScene_Change::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iSpriteIndex, _bool bReversePlay)
{
	CScene_Change*	pInstance = new CScene_Change(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iSpriteIndex, bReversePlay)))
	{
		MSG_BOX("Failed to Create : CScene_Change");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CScene_Change::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CScene_Change*	pInstance = new CScene_Change(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CScene_Change");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Change::Free()
{
	__super::Free();
}
