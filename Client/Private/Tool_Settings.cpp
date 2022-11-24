#include "stdafx.h"
#include "..\Public\Tool_Settings.h"
#include "GameInstance.h"

HRESULT CTool_Settings::Initialize(void * pArg)
{
	m_szWIndowName = "ImGui Setting";
	m_pAlpha = &ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w;

	return S_OK;
}

void CTool_Settings::ImGui_RenderWindow()
{
	IMGUI_LEFT_LABEL(ImGui::SliderFloat, "Alpha", m_pAlpha, 0.f, 1.f);
	ImGui::Text("(CTRL+Click to Input Directly)");
}

CTool_Settings * CTool_Settings::Create(void * pArg)
{
	CTool_Settings*		pInstance = new CTool_Settings();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_Settings");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Settings::Free()
{
}