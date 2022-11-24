#include "stdafx.h"
#include "..\Public\Tool_Debugger.h"
#include "GameInstance.h"

HRESULT CTool_Debugger::Initialize(void * pArg)
{
	m_szWIndowName = "Debugger";
	m_fFrameRate = &ImGui::GetIO().Framerate;
	m_TextColor = { 0.f, 0.f, 0.f, 1.f };

	return S_OK;
}

void CTool_Debugger::ImGui_RenderWindow()
{
	if (*m_fFrameRate >= 60.f)
		m_TextColor = { 0.f, 1.f, 0.f, 1.f };
	else if (*m_fFrameRate >= 30.f && *m_fFrameRate < 60.f)
		m_TextColor = { 1.f, 0.7f, 0.f, 1.f };
	else
		m_TextColor = { 1.f, 0.f, 0.f, 1.f };

	ImGui::TextColored(m_TextColor, "FPS : %.1f", ImGui::GetIO().Framerate);
}

CTool_Debugger * CTool_Debugger::Create(void * pArg)
{
	CTool_Debugger*		pInstance = new CTool_Debugger();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_Debugger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Debugger::Free()
{
}
