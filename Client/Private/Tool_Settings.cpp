#include "stdafx.h"
#include "..\Public\Tool_Settings.h"
#include "GameInstance.h"
#include "Camera.h"

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

	_uint		iCurrentLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();

	if (iCurrentLevel == 2)
	{
		_bool&		DynamicCamera = dynamic_cast<CCamera*>(CGameInstance::GetInstance()->Get_CloneObjectList(iCurrentLevel, L"Layer_Camera")->front())->Get_RenderState();
		_bool&		StaticCamera = dynamic_cast<CCamera*>(CGameInstance::GetInstance()->Get_CloneObjectList(iCurrentLevel, L"Layer_Camera")->back())->Get_RenderState();
		_bool		bSelect = false;

		if (ImGui::RadioButton("Free Cam", bSelect))
		{
			bSelect = !bSelect;
			DynamicCamera = true;
			StaticCamera = false;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Player Cam", !bSelect))
		{
			bSelect = !bSelect;
			DynamicCamera = false;
			StaticCamera = true;
		}
	}
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