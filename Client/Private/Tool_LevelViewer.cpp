#include "stdafx.h"
#include "..\Public\Tool_LevelViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CTool_LevelViewer::CTool_LevelViewer(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTool_LevelViewer::Initialize(void * pArg)
{
	m_szWIndowName = "Level Viewer";

	return S_OK;
}

void CTool_LevelViewer::ImGui_RenderWindow()
{
	m_iCurrentLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();
	ImGui::Text("Current Level : %s", m_pLevelName[m_iCurrentLevel]);

	ImGui::NewLine();
	ImGui::BulletText("Change Level");
	ImGui::Combo(" ", &m_iNextLevel, m_pLevelName, LEVEL_END);
	ImGui::SameLine();
	if (ImGui::Button("Change"))
	{
		if (m_iNextLevel == LEVEL_LOADING || m_iCurrentLevel == m_iNextLevel)
			return;

		FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVEL)m_iNextLevel)), );
	}
}

CTool_LevelViewer * CTool_LevelViewer::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void * pArg)
{
	CTool_LevelViewer*		pInstance = new CTool_LevelViewer(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_LevelViewer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_LevelViewer::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
