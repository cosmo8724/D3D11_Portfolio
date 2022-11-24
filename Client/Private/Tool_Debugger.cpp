#include "stdafx.h"
#include "..\Public\Tool_Debugger.h"
#include "GameInstance.h"

HRESULT CTool_Debugger::Initialize(void * pArg)
{
	m_szWIndowName = "Debugger";

	return S_OK;
}

void CTool_Debugger::ImGui_RenderWindow()
{
	ImGui::Text("FPS : %.1f", ImGui::GetIO().Framerate);
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
