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

		_matrix	matWorld = m_pTransformCom->Get_WorldMatrix();
		m_pTransformCom->Set_WorldMatrix(matWorld * XMMatrixScaling(m_fAspectRatioX, m_fAspectRatioY, 1.f));
	}
	else
	{
		m_fAspectRatioX = 1.f;
		m_fAspectRatioY = 1.f;
	}

	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
}

void CUI::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_pParent != nullptr)
	{
		_matrix	matWorld, matParentWorld;
		matWorld = m_pTransformCom->Get_WorldMatrix();
		matParentWorld = m_pParent->Get_WorldMatrix();

		memcpy(&matParentWorld.r[0], &XMVector3Normalize(matParentWorld.r[0]), sizeof(_vector));
		memcpy(&matParentWorld.r[1], &XMVector3Normalize(matParentWorld.r[1]), sizeof(_vector));
		memcpy(&matParentWorld.r[2], &XMVector3Normalize(matParentWorld.r[2]), sizeof(_vector));

		m_matWorldmulParent = matWorld * matParentWorld;
	}
}

HRESULT CUI::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	return S_OK;
}

CUI * CUI::Find_UI(_uint iLevelIndex, const wstring & wstrUITag)
{
	auto	pCloneUIList = CGameInstance::GetInstance()->Get_CloneObjectList(iLevelIndex, L"Layer_UI");
	NULL_CHECK_RETURN(pCloneUIList, nullptr);

	for (auto pUI : *pCloneUIList)
	{
		if (dynamic_cast<CUI*>(pUI)->Get_UITag() == wstrUITag)
			return dynamic_cast<CUI*>(pUI);
	}

	return nullptr;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
