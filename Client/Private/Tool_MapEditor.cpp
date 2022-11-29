#include "stdafx.h"
#include "..\Public\Tool_MapEditor.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "GameUtility.h"

CTool_MapEditor::CTool_MapEditor()
{
}

HRESULT CTool_MapEditor::Initialize(void * pArg)
{
	m_szWIndowName = "Map Editor";

	m_mapPrototypes = CGameInstance::GetInstance()->Get_Prototypes();
	m_iPrototypeCnt = (_uint)m_mapPrototypes->size();

	m_iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();
	m_mapLayers = CGameInstance::GetInstance()->Get_Layers(m_iCurLevel);

	for (auto Pair : *m_mapPrototypes)
	{
		if (Pair.second->Get_HasModel())
			m_mapPrototypeModels.emplace(Pair.first, Pair.second);
	}

	return S_OK;
}

void CTool_MapEditor::ImGui_RenderWindow()
{
	CheckLevel();
	CheckNewPrototype();

	static _int	iSelectObject = -1;
	size_t i2 = m_mapPrototypeModels.size();

	if (m_mapPrototypeModels.size())
	{
		char**			ppProtoModelTag = new char*[m_mapPrototypeModels.size()];
		_uint			i = 0;
		for (auto Pair : m_mapPrototypeModels)
		{
			ppProtoModelTag[i] = new char[Pair.first.length() + 1];
			CGameUtility::wctc(Pair.first.c_str(), ppProtoModelTag[i]);
			i++;
		}

		ImGui::BulletText("Prototype Model List");
		ImGui::ListBox("Prototype Model List", &iSelectObject, ppProtoModelTag, (_int)m_mapPrototypeModels.size());

		if (iSelectObject != -1)
		{
			static _int	iSelectLayer = 0;
			char**			ppLayerTags = new char*[m_mapLayers->size()];
			_uint			j = 0;
			for (auto Pair : *m_mapLayers)
			{
				ppLayerTags[j] = new char[Pair.first.length() + 1];
				CGameUtility::wctc(Pair.first.c_str(), ppLayerTags[j]);
				j++;
			}

			ImGui::Separator();
			IMGUI_LEFT_LABEL(ImGui::Combo, "Target Layer", &iSelectLayer, ppLayerTags, (_int)m_mapLayers->size());

			ImGui::SameLine();
			if (ImGui::Button("Clone"))
			{
				_tchar		wszBuff[MAX_PATH] = L"";
				CGameUtility::ctwc(ppProtoModelTag[iSelectObject], wszBuff);
				wstring	wstrPrototypeTag = wszBuff;
				CGameUtility::ctwc(ppLayerTags[iSelectLayer], wszBuff);
				wstring	wstrLayerTag = wszBuff;

				CGameInstance::GetInstance()->Clone_GameObject(m_iCurLevel, wstrLayerTag, wstrPrototypeTag);
			}

			for (size_t i = 0; i < m_mapLayers->size(); ++i)
				Safe_Delete_Array(ppLayerTags[i]);
			Safe_Delete_Array(ppLayerTags);
		}

		for (size_t i = 0; i < m_mapPrototypeModels.size(); ++i)
			Safe_Delete_Array(ppProtoModelTag[i]);
		Safe_Delete_Array(ppProtoModelTag);
	}
	else
		ImGui::Text("There is no Prototype Model.");
}

void CTool_MapEditor::CheckNewPrototype()
{
	if (m_iPrototypeCnt == m_mapPrototypes->size())
		return;

	m_iPrototypeCnt = (_uint)m_mapPrototypes->size();

	for (auto Pair : *m_mapPrototypes)
	{
		if (!Pair.second->Get_HasModel())
			continue;

		auto iter = find_if(m_mapPrototypeModels.begin(), m_mapPrototypeModels.end(), CTag_Finder(Pair.first));

		if (iter == m_mapPrototypeModels.end())
			m_mapPrototypeModels.emplace(Pair.first, Pair.second);
	}
}

void CTool_MapEditor::CheckLevel()
{
	if (m_iCurLevel != CGameInstance::GetInstance()->Get_CurLevelIndex())
	{
		m_iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();
		m_mapLayers = CGameInstance::GetInstance()->Get_Layers(m_iCurLevel);
	}
}

CTool_MapEditor * CTool_MapEditor::Create(void * pArg)
{
	CTool_MapEditor*	pInstance = new CTool_MapEditor();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_MapEditor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_MapEditor::Free()
{
	m_mapPrototypeModels.clear();
}
