#include "stdafx.h"
#include "..\Public\Talk_ButtonE.h"
#include "GameInstance.h"

CTalk_ButtonE::CTalk_ButtonE(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_ButtonE::CTalk_ButtonE(const CTalk_ButtonE & rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_ButtonE::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTalk_ButtonE::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	return S_OK;
}

void CTalk_ButtonE::Tick(_double dTimeDelta)
{
}

void CTalk_ButtonE::Late_Tick(_double dTimeDelta)
{
}

HRESULT CTalk_ButtonE::Render()
{
	return S_OK;
}

void CTalk_ButtonE::ImGui_RenderProperty()
{
}

HRESULT CTalk_ButtonE::SetUp_Component()
{
	return S_OK;
}

HRESULT CTalk_ButtonE::SetUp_Parts()
{
	return S_OK;
}

HRESULT CTalk_ButtonE::SetUp_ShaderResources()
{
	return S_OK;
}

CTalk_ButtonE * CTalk_ButtonE::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTalk_ButtonE*	pInstance = new CTalk_ButtonE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTalk_ButtonE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk_ButtonE::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTalk_ButtonE*	pInstance = new CTalk_ButtonE(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CTalk_ButtonE");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalk_ButtonE::Free()
{
	__super::Free();
}
