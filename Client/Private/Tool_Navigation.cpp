#include "stdafx.h"
#include "..\Public\Tool_Navigation.h"
#include "GameInstance.h"
#include "CustomGameObject.h"
#include "GameUtility.h"
#include "Json/json.hpp"
#include <fstream>
#include "Cell.h"
#include "Ocean.h"

CTool_Navigation::CTool_Navigation(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTool_Navigation::Initialize(void * pArg)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_szWIndowName = "Navigation Editor";

	m_pNavigationCom = CNavigation::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CTool_Navigation::ImGui_RenderWindow()
{
	char**			ppCellTag = nullptr;
	static _bool	bSave = false;
	static char	szSaveFileName[MAX_PATH] = "";

	NULL_CHECK_RETURN(m_pNavigationCom, );

//#ifdef _DEBUG
	for (_uint i = 0; i < (_uint)POINT_END; ++i)
	{
		if (m_pVIBufferCircleCom[i] != nullptr)
			Safe_Release(m_pVIBufferCircleCom[i]);

		if (!XMVector3Equal(XMVectorSetW(XMLoadFloat3(&m_vPoint[i]), 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f)))
			m_pVIBufferCircleCom[i] = CVIBuffer_Cell_Circle::Create(m_pDevice, m_pContext, m_vPoint[i]);
	}
//#endif // _DEBUG

	m_iNumCell = m_pNavigationCom->Get_CellCount();
	ImGui::BulletText("Cell Count : %d", m_iNumCell);
	ImGui::BulletText("Cell List");

	if (m_iNumCell == 0)
		ImGui::Text("There is No Cell.");
	else
	{
		ppCellTag = new char*[m_iNumCell];
		for (_uint i = 0; i < m_iNumCell; ++i)
		{
			char		pCellTag[128] = "";
			sprintf_s(pCellTag, "Cell_%d", i);
			ppCellTag[i] = new char[strlen(pCellTag) + 1];
			strcpy_s(ppCellTag[i], strlen(pCellTag) + 1, pCellTag);
		}

		ImGui::ListBox("Cell List", &m_iSelectedCell, ppCellTag, m_iNumCell);

		if ((ImGui::Button("Delete") || m_pGameInstance->Key_Down(DIK_DELETE)) && m_iSelectedCell != -1)
		{
			for (_uint i = 0; i < m_iNumCell; ++i)
				Safe_Delete_Array(ppCellTag[i]);
			Safe_Delete_Array(ppCellTag);

			m_pNavigationCom->Delete_Cell(m_iSelectedCell);

			m_iSelectedCell = -1;
			m_iPickingCell = -1;

			return;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Save") && m_iNumCell != 0)
		bSave = true;
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		ImGuiFileDialog::Instance()->OpenDialog("Load Navigation", "Load Navigation", ".json", "../Bin/Save Data/Navigation", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

	if (bSave)
	{
		ImGui::InputText("File Name", szSaveFileName, MAX_PATH);
		ImGui::SameLine();
		if (ImGui::Button("Confirm"))
			ImGuiFileDialog::Instance()->OpenDialog("Save Navigation", "Save Navigation", ".json", "../Bin/Save Data/Navigation", strcat(szSaveFileName, ".json"), 0, nullptr, ImGuiFileDialogFlags_Modal);

		if (ImGuiFileDialog::Instance()->Display("Save Navigation"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				string		strSaveFileDirectory = ImGuiFileDialog::Instance()->GetCurrentPath();
				char	szDash[128] = "\\";
				strcat_s(szDash, szSaveFileName);
				strSaveFileDirectory += string(szDash);

				m_pNavigationCom->Save_Cell(strSaveFileDirectory);

				strcpy_s(szSaveFileName, "");
				bSave = false;

				ImGuiFileDialog::Instance()->Close();
			}
			if (!ImGuiFileDialog::Instance()->IsOk())
				ImGuiFileDialog::Instance()->Close();
		}
	}

	if (ImGuiFileDialog::Instance()->Display("Load Navigation"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			for (_uint i = 0; i < m_iNumCell; ++i)
				Safe_Delete_Array(ppCellTag[i]);
			Safe_Delete_Array(ppCellTag);

			Safe_Release(m_pNavigationCom);

			string		strFilePath = ImGuiFileDialog::Instance()->GetFilePathName();;
			wstring	wstrFilePath = L"";
			wstrFilePath.assign(strFilePath.begin(), strFilePath.end());

			_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szSaveFileName, MAX_PATH, nullptr, 0);

			m_pNavigationCom = CNavigation::Create(m_pDevice, m_pContext, wstrFilePath);

			m_iSelectedCell = -1;

			ImGuiFileDialog::Instance()->Close();

			return;
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
			ImGuiFileDialog::Instance()->Close();
	}

	ImGui::Separator();
	static _bool	bDrawCell = false;
	static _bool	bPickingCell = false;
	ImGui::Checkbox("Draw Cell", &bDrawCell);
	ImGui::SameLine();
	ImGui::Checkbox("Picking Cell", &bPickingCell);

	if (m_pGameInstance->Get_CurLevelIndex() != LEVEL_TESTSTAGE)
		bDrawCell = false;

	if (m_pNavigationCom == nullptr || m_iNumCell == 0)
		bPickingCell = false;

	if (bDrawCell && m_pGameInstance->Get_CurLevelIndex() == LEVEL_TESTSTAGE)
	{
		static _bool	bMeshType = true;
		ImGui::BulletText("Mesh Type");
		if (ImGui::RadioButton("Island", bMeshType))
			bMeshType = true;
		ImGui::SameLine();
		if (ImGui::RadioButton("Stone", !bMeshType))
			bMeshType = false;

		// Terrain Picking
		if (m_pGameInstance->Mouse_Down(DIM_LB) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		{
			COcean*		pOcean = dynamic_cast<COcean*>(m_pGameInstance->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Ocean")->front());
			NULL_CHECK_RETURN(pOcean, );

			pair<_bool, _float3>		PickInfo = pOcean->Picking_Terrain();

			if (PickInfo.first == true)
			{				
				m_pNavigationCom->Find_NearBy_Point(PickInfo.second);

				if (m_iCurPoint == (_uint)POINT_A)
				{
					if (PickInfo.second != m_vPoint[POINT_C])
					{
						m_vPoint[m_iCurPoint] = PickInfo.second;
						m_iCurPoint++;
					}
				}
				else
				{
					if (PickInfo.second != m_vPoint[m_iCurPoint - 1])
					{
						m_vPoint[m_iCurPoint] = PickInfo.second;
						m_iCurPoint++;
					}
				}

				if (m_iCurPoint == (_uint)POINT_END)
				{
					CGameUtility::SortPointsByCW(m_vPoint);
					m_pNavigationCom->Add_Cell(m_vPoint);

					m_iCurPoint = (_uint)POINT_A;
					ZeroMemory(m_vPoint, sizeof(_float3) * POINT_END);
				}
			}
		}
		// Mesh Picking
		else if (m_pGameInstance->Mouse_Down(DIM_RB) && m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		{
			if (bMeshType == true)
			{
				list<CGameObject*>*	IslandList = m_pGameInstance->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Islands");

				for (auto& pIsland : *IslandList)
				{
					pair<_bool, _float3>		PickInfo = pIsland->Picking_Mesh();

					if (PickInfo.first == true)
					{
						m_pNavigationCom->Find_NearBy_Point(PickInfo.second);

						if (m_iCurPoint == (_uint)POINT_A)
						{
							if (PickInfo.second != m_vPoint[POINT_C])
							{
								m_vPoint[m_iCurPoint] = PickInfo.second;
								m_iCurPoint++;
							}
						}
						else
						{
							if (PickInfo.second != m_vPoint[m_iCurPoint - 1])
							{
								m_vPoint[m_iCurPoint] = PickInfo.second;
								m_iCurPoint++;
							}
						}

						if (m_iCurPoint == (_uint)POINT_END)
							break;
					}
				}
			}
			else if (bMeshType == false)
			{
				list<CGameObject*>*	StoneList = m_pGameInstance->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Stones");

				for (auto& pObj : *StoneList)
				{
					CCustomGameObject*	pStone = dynamic_cast<CCustomGameObject*>(pObj);
					if (pStone == nullptr)
						continue;

					pair<_bool, _float3>		PickInfo = pStone->Picking_Mesh(g_hWnd);

					if (PickInfo.first == true)
					{
						m_pNavigationCom->Find_NearBy_Point(PickInfo.second);

						if (m_iCurPoint == (_uint)POINT_A)
						{
							if (PickInfo.second != m_vPoint[POINT_C])
							{
								m_vPoint[m_iCurPoint] = PickInfo.second;
								m_iCurPoint++;
							}
						}
						else
						{
							if (PickInfo.second != m_vPoint[m_iCurPoint - 1])
							{
								m_vPoint[m_iCurPoint] = PickInfo.second;
								m_iCurPoint++;
							}
						}

						if (m_iCurPoint == (_uint)POINT_END)
							break;
					}
				}
			}

			if (m_iCurPoint >= (_uint)POINT_END)
			{
				CGameUtility::SortPointsByCW(m_vPoint);
				m_pNavigationCom->Add_Cell(m_vPoint);

				m_iCurPoint = (_uint)POINT_A;
				ZeroMemory(m_vPoint, sizeof(_float3) * POINT_END);
			}
		}
		// Undo
		else if (m_pGameInstance->Key_Down(DIK_Z) && m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80)
		{
			if (m_iCurPoint == (_uint)POINT_A)
			{
				if (m_iNumCell != 0)
				{
					if (m_iSelectedCell == m_iNumCell - 1)
						m_iSelectedCell = -1;

					if (bPickingCell ==  true)
						m_iPickingCell = -1;

					CCell*		pCell = m_pNavigationCom->Get_Cell(m_iNumCell - 1);
					NULL_CHECK_RETURN(pCell, );

					m_vPoint[POINT_A] = pCell->Get_Point(CCell::POINT_A);
					m_vPoint[POINT_B] = pCell->Get_Point(CCell::POINT_B);

					m_pNavigationCom->Delete_Cell(m_iNumCell - 1);

					m_iCurPoint = (_uint)POINT_C;
				}
			}
			else
				m_iCurPoint--;

			m_vPoint[m_iCurPoint] = { 0.f, 0.f, 0.f };
		}

		ImGui::InputFloat3("POINT_A", &m_vPoint[POINT_A].x);
		ImGui::InputFloat3("POINT_B", &m_vPoint[POINT_B].x);
		ImGui::InputFloat3("POINT_C", &m_vPoint[POINT_C].x);
	}

	if (bPickingCell)
	{
		pair<_bool, _int>		Result = CGameUtility::Cell_Picking(g_hWnd, (_float)g_iWinSizeX, (_float)g_iWinSizeY, m_pNavigationCom);

		if (Result.first == true)
		{
			m_iPickingCell = Result.second;

			if (m_pGameInstance->GetInstance()->Mouse_Down(DIM_LB))
				m_iSelectedCell = Result.second;
		}
	}
	else
		m_iPickingCell = -1;

	if (m_iSelectedCell != -1)
	{
		ImGui::Separator();

		CCell*		pCell = m_pNavigationCom->Get_Cell(m_iSelectedCell);
		NULL_CHECK_RETURN(pCell, );

		pCell->ImGui_RenderProperty();

		if (m_pGameInstance->Key_Down(DIK_1))
			pCell->Set_State(CCell::STATE_OCEAN);

		if (m_pGameInstance->Key_Down(DIK_2))
			pCell->Set_State(CCell::STATE_GROUND);

		if (m_pGameInstance->Key_Down(DIK_3))
			pCell->Set_State(CCell::STATE_WALL);

		if (m_pGameInstance->Key_Down(DIK_4))
			pCell->Set_State(CCell::STATE_ROOF);

		if (m_pGameInstance->Key_Down(DIK_5))
			pCell->Set_State(CCell::STATE_AIR);

		if (m_pGameInstance->Key_Down(DIK_ESCAPE))
			m_iSelectedCell = -1;
	}

	for (_uint i = 0; i < m_iNumCell; ++i)
		Safe_Delete_Array(ppCellTag[i]);
	Safe_Delete_Array(ppCellTag);
}

void CTool_Navigation::Render()
{
//#ifdef _DEBUG
	m_pNavigationCom->Render();

	if (m_iSelectedCell != -1)
		m_pNavigationCom->Render_Selected_Cell(m_iSelectedCell, _float4(1.f, 0.4f, 0.f, 1.f));

	if (m_iPickingCell != -1)
		m_pNavigationCom->Render_Selected_Cell(m_iPickingCell, _float4(0.3f, 0.3f, 0.3f, 1.f));

	_float		fHeight = 0.15f;
	_float4	vColor = { 1.f, 0.4f, 0.f, 1.f };
	m_pNavigationCom->Change_Render_HeightColor(fHeight, vColor);
	
	for (_uint i = 0; i < (_uint)POINT_END; ++i)
	{
		if (m_pVIBufferCircleCom[i] != nullptr)
			m_pVIBufferCircleCom[i]->Render();
	}
//#endif // _DEBUG
}

CTool_Navigation * CTool_Navigation::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void * pArg)
{
	CTool_Navigation*	pInstance = new CTool_Navigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_Navigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_Navigation::Free()
{
//#ifdef _DEBUG
	for (_uint i = 0; i < (_uint)POINT_END; ++i)
		Safe_Release(m_pVIBufferCircleCom[i]);
//#endif // _DEBUG

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}