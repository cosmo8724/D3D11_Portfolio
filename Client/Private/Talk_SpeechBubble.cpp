#include "stdafx.h"
#include "..\Public\Talk_SpeechBubble.h"
#include "GameInstance.h"

CTalk_SpeechBubble::CTalk_SpeechBubble(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CUI(pDevice, pContext)
{
}

CTalk_SpeechBubble::CTalk_SpeechBubble(const CTalk_SpeechBubble & rhs)
	: CUI(rhs)
{
}

HRESULT CTalk_SpeechBubble::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTalk_SpeechBubble::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	return S_OK;
}

void CTalk_SpeechBubble::Tick(_double dTimeDelta)
{
}

void CTalk_SpeechBubble::Late_Tick(_double dTimeDelta)
{
}

HRESULT CTalk_SpeechBubble::Render()
{
	return S_OK;
}

void CTalk_SpeechBubble::ImGui_RenderProperty()
{
}

HRESULT CTalk_SpeechBubble::SetUp_Component()
{
	return S_OK;
}

HRESULT CTalk_SpeechBubble::SetUp_Parts()
{
	return S_OK;
}

HRESULT CTalk_SpeechBubble::SetUp_ShaderResources()
{
	return S_OK;
}

CTalk_SpeechBubble * CTalk_SpeechBubble::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CTalk_SpeechBubble*	pInstance = new CTalk_SpeechBubble(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTalk_SpeechBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalk_SpeechBubble::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CTalk_SpeechBubble*	pInstance = new CTalk_SpeechBubble(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Create : CTalk_SpeechBubble");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalk_SpeechBubble::Free()
{
	__super::Free();
}
