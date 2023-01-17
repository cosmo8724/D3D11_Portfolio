#include "stdafx.h"
#include "..\Public\UI.h"
#include "GameInstance.h"

CUI::CUI(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CUI::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	m_iLastWinSizeX = g_iWinSizeX;
	m_iLastWinSizeY = g_iWinSizeY;

	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());

	return S_OK;
}

void CUI::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (g_bNeedResizeSwapChain == true)
	{
		m_fAspectRatioX = (_float)g_iWinSizeX / (_float)m_iLastWinSizeX;
		m_fAspectRatioY = (_float)g_iWinSizeY / (_float)m_iLastWinSizeY;

		m_iLastWinSizeX = g_iWinSizeX;
		m_iLastWinSizeY = g_iWinSizeY;

		m_fSizeX *= m_fAspectRatioX;
		m_fSizeY *= m_fAspectRatioY;
		m_fX = m_fSizeX * 0.5f;
		m_fY = m_fSizeY * 0.5f;

		_matrix	matWorld = m_pTransformCom->Get_WorldMatrix();
		m_pTransformCom->Set_WorldMatrix(matWorld * XMMatrixScaling(m_fAspectRatioX, m_fAspectRatioY, 1.f));
	}
	else
	{
		m_fAspectRatioX = 1.f;
		m_fAspectRatioY = 1.f;
	}

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}

void CUI::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);
}

HRESULT CUI::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
