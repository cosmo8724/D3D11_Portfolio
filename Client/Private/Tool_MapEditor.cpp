#include "stdafx.h"
#include "..\Public\Tool_MapEditor.h"
#include "GameInstance.h"
#include "Layer.h"
#include "GameUtility.h"
#include "CustomGameObject.h"
#include "Sigrid.h"
#include "Enemy.h"
#include "NPC.h"

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
	_bool	bLevelChanged = CheckLevel();
	CheckNewPrototype();

	static _int	iSelectObject = -1;
	static _int	iSelectLayer = 0;
	static	_int iSelectLayer2 = 0;

	if (bLevelChanged)
	{
		iSelectObject = -1;
		iSelectLayer = 0;
		iSelectLayer2 = 0;
	}

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

		if (m_iCurLevel == LEVEL_LOADING)
		{
			ImGui::Text("Wait for Loading Complete.");

			for (size_t i = 0; i < m_mapPrototypeModels.size(); ++i)
				Safe_Delete_Array(ppProtoModelTag[i]);
			Safe_Delete_Array(ppProtoModelTag);

			return;
		}

		if (iSelectObject != -1)
		{
			static char	szLayerTag[MAX_PATH] = "";

			IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "Add Layer   ", "Input LayerTag here and Press Add Button", szLayerTag, MAX_PATH);
			ImGui::SameLine();
			if (ImGui::Button("Add") && strcmp(szLayerTag, ""))
			{
				_tchar		wszLayerTag[MAX_PATH] = L"";
				CGameUtility::ctwc(szLayerTag, wszLayerTag);
				CGameInstance::GetInstance()->Add_Layer(m_iCurLevel, wstring(wszLayerTag));
				ZeroMemory(szLayerTag, MAX_PATH);
			}

			char**			ppLayerTags = new char*[m_mapLayers->size()];
			_uint			j = 0;
			for (auto Pair : *m_mapLayers)
			{
				ppLayerTags[j] = new char[Pair.first.length() + 1];
				CGameUtility::wctc(Pair.first.c_str(), ppLayerTags[j]);
				j++;
			}

			IMGUI_LEFT_LABEL(ImGui::Combo, "Target Layer", &iSelectLayer, ppLayerTags, (_int)m_mapLayers->size());

			ImGui::SameLine();
			if (ImGui::Button("Clone"))
			{
				_tchar		wszBuff[MAX_PATH] = L"";
				CGameUtility::ctwc(ppProtoModelTag[iSelectObject], wszBuff);
				wstring	wstrPrototypeTag = wszBuff;
				CGameUtility::ctwc(ppLayerTags[iSelectLayer], wszBuff);
				wstring	wstrLayerTag = wszBuff;

				CGameObject*	pGameObject = CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(m_iCurLevel, wstrLayerTag, wstrPrototypeTag);

				if (CEnemy* pEnemy = dynamic_cast<CEnemy*>(pGameObject))
				{
					CSigrid*	pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(m_iCurLevel, L"Layer_Player")->back());
					pEnemy->Set_Player(pPlayer);
				}

				if (CNPC*		pNPC = dynamic_cast<CNPC*>(pGameObject))
				{
					CSigrid*	pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(m_iCurLevel, L"Layer_Player")->back());
					pNPC->Set_Player(pPlayer);
				}
			}

			ImGui::Separator();

			static _int iLastSelectedLayer = iSelectLayer2;
			static _int iSelectCloneObject = -1;
			ImGui::BulletText("Layer Viewer");
			IMGUI_LEFT_LABEL(ImGui::Combo, "Select Layer", &iSelectLayer2, ppLayerTags, (_int)m_mapLayers->size());

			if (iSelectLayer2 != iLastSelectedLayer)
			{
				iSelectCloneObject = -1;
				iLastSelectedLayer = iSelectLayer2;
			}

			_tchar		wszLayerTag[MAX_PATH] = L"";
			CGameUtility::ctwc(ppLayerTags[iSelectLayer2], wszLayerTag);

			list<CGameObject*>*	CloneObjectList = CGameInstance::GetInstance()->Get_CloneObjectList(m_iCurLevel, wstring(wszLayerTag));

			char**		ppCloneTags = new char*[CloneObjectList->size()];
			wstring	wstrLastTag = L"";
			_tchar		wszBuff[128] = L"";
			CGameUtility::ctwc(ppLayerTags[iSelectLayer2], wszBuff);
			CGameUtility::SplitTag(wszBuff, wstrLastTag);
			char		szLastTag[128] = "";

			_uint i = 0;
			for (auto iter : *CloneObjectList)
			{
				wsprintf(wszBuff, &(*iter->Get_PrototypeGameObjectTag().c_str()));
				CGameUtility::SplitTag(wszBuff, wstrLastTag);
				CGameUtility::wctc(wstrLastTag.c_str(), szLastTag);
				ppCloneTags[i] = new char[MAX_PATH];
				sprintf_s(ppCloneTags[i], sizeof(char) * MAX_PATH, strcat(szLastTag, "_%d"), i);
				i++;
			}

			/*for (size_t i = 0; i < CloneObjectList->size(); ++i)
			{
				ppCloneTags[i] = new char[MAX_PATH];
				CGameUtility::wctc(wstrLastTag.c_str(), szLastTag);
				sprintf_s(ppCloneTags[i], sizeof(char) * MAX_PATH, strcat(szLastTag, "_%d"), i);
			}*/
			ImGui::ListBox("Clone Model List", &iSelectCloneObject, ppCloneTags, (_int)CloneObjectList->size());

			if (CGameInstance::GetInstance()->Mouse_Down(DIM_RB))
				iSelectCloneObject = -1;

			if (iSelectCloneObject != -1)
			{
				_bool	bClickedAnyButton = false;
				auto	iter = CloneObjectList->begin();

				if (ImGui::Button("Delete") || CGameInstance::GetInstance()->Key_Down(DIK_DELETE))
				{
					bClickedAnyButton = true;

					for (_int i = 0; i < iSelectCloneObject; ++i)
						iter++;

					Safe_Release(*iter);
					CloneObjectList->erase(iter);
					Safe_Delete_Array(ppCloneTags[iSelectCloneObject]);

					/* 중간 삭제 시, 맨 마지막 문자열 또한 지워줌. */
					if (iSelectCloneObject != CloneObjectList->size() + 1)
						Safe_Delete_Array(ppCloneTags[CloneObjectList->size()]);

					iSelectCloneObject = -1;
				}
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					bClickedAnyButton = true;

				}
				ImGui::SameLine();
				if (ImGui::Button("Load"))
				{
					bClickedAnyButton = true;

				}
				if (!bClickedAnyButton)
				{
					ImGui::Separator();

					iter = CloneObjectList->begin();
					for (_int i = 0; i < iSelectCloneObject; ++i)
						iter++;

					CGameObject*	pGameObject = *iter;
					
					ImGui::BulletText("Current Selected Object : %s", ppCloneTags[iSelectCloneObject]);

					if (pGameObject->Get_HasModel() && dynamic_cast<CCustomGameObject*>(pGameObject))
					{
						static _int	iCurrentAnimation = 0;
						_uint	iAnimationCnt = dynamic_cast<CCustomGameObject*>(pGameObject)->Get_ModelComponent()->Get_NumAnimations();

						ImGui::NewLine();
						ImGui::BulletText("Current Animation : %d", iCurrentAnimation);
						ImGui::SameLine();
						if (ImGui::SmallButton("<"))
							iCurrentAnimation--;
						ImGui::SameLine();
						if (ImGui::SmallButton(">"))
							iCurrentAnimation++;
						CGameUtility::Saturate(iCurrentAnimation, iAnimationCnt, 0);

						dynamic_cast<CCustomGameObject*>(pGameObject)->Get_ModelComponent()->Set_CurAnimationIndex(iCurrentAnimation);
					}

					ImGuizmo::BeginFrame();

					const _float4x4&	matWorld = pGameObject->Get_WorldMatrix();
					static ImGuizmo::OPERATION CurGuizmoType(ImGuizmo::TRANSLATE);

					ImGui::Text("ImGuizmo Type");
					if (ImGui::RadioButton("Translate", CurGuizmoType == ImGuizmo::TRANSLATE))
						CurGuizmoType = ImGuizmo::TRANSLATE;
					ImGui::SameLine();
					if (ImGui::RadioButton("Scale", CurGuizmoType == ImGuizmo::SCALE))
						CurGuizmoType = ImGuizmo::SCALE;
					ImGui::SameLine();
					if (ImGui::RadioButton("Rotate", CurGuizmoType == ImGuizmo::ROTATE))
						CurGuizmoType = ImGuizmo::ROTATE;
					ImGui::Separator();

					_float	vPos[3], vScale[3], vAngle[3];
					ImGuizmo::DecomposeMatrixToComponents((_float*)&matWorld, vPos, vAngle, vScale);
					IMGUI_LEFT_LABEL(ImGui::InputFloat3, "Translate", vPos);
					IMGUI_LEFT_LABEL(ImGui::InputFloat3, "Scale", vScale);
					IMGUI_LEFT_LABEL(ImGui::InputFloat3, "Rotate", vAngle);
					ImGuizmo::RecomposeMatrixFromComponents(vPos, vAngle, vScale, (_float*)&matWorld);

					ImGuiIO&	io = ImGui::GetIO();
					RECT		rt;
					GetClientRect(g_hWnd, &rt);
					POINT		LT{ rt.left, rt.top };
					ClientToScreen(g_hWnd, &LT);
					ImGuizmo::SetRect((_float)LT.x, (_float)LT.y, io.DisplaySize.x, io.DisplaySize.y);

					_float4x4		matView, matProj;
					XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
					XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

					ImGuizmo::Manipulate((_float*)&matView, (_float*)&matProj, CurGuizmoType, ImGuizmo::WORLD, (_float*)&matWorld);

					ImGui::Separator();
					pGameObject->ImGui_RenderComponentProperties();
				}
			}


			for (size_t i = 0; i < CloneObjectList->size(); ++i)
				Safe_Delete_Array(ppCloneTags[i]);
			Safe_Delete_Array(ppCloneTags);

			for (size_t i = 0; i < m_mapLayers->size(); ++i)
				Safe_Delete_Array(ppLayerTags[i]);
			Safe_Delete_Array(ppLayerTags);
		}

		for (size_t i = 0; i < m_mapPrototypeModels.size(); ++i)
			Safe_Delete_Array(ppProtoModelTag[i]);
		Safe_Delete_Array(ppProtoModelTag);
	}
	else
		ImGui::Text("There is no Prototype Model.\nPlease Add Prototype Model First.");
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

_bool CTool_MapEditor::CheckLevel()
{
	if (m_iCurLevel != CGameInstance::GetInstance()->Get_CurLevelIndex())
	{
		m_iCurLevel = CGameInstance::GetInstance()->Get_CurLevelIndex();
		m_mapLayers = CGameInstance::GetInstance()->Get_Layers(m_iCurLevel);
		return true;
	}

	return false;
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
