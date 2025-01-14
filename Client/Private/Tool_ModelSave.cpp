#include "stdafx.h"
#include "..\Public\Tool_ModelSave.h"
#include "ComponentMgr.h"

CTool_ModelSave::CTool_ModelSave(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTool_ModelSave::Initialize(void * pArg)
{
	m_szWIndowName = "FBX to Model Converter";

	return S_OK;
}

void CTool_ModelSave::ImGui_RenderWindow()
{
	static char	szModelFilePath[MAX_PATH] = "";
	static CModel::MODELTYPE		eModelType = CModel::MODELTYPE_END;
	char*	szModelTypes[CModel::MODELTYPE_END] = { "Non-Animation", "Animation" };

	IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "File Path", "Input Path here or Choose from FileDialog", szModelFilePath, sizeof(char) * MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Open"))
		ImGuiFileDialog::Instance()->OpenDialog("Choose FBX", "Select FBX Model File", ".fbx", "../Bin/Resource/Mesh", ".fbx", 1, nullptr, ImGuiFileDialogFlags_Modal);

	IMGUI_LEFT_LABEL(ImGui::Combo, "Model Type", (_int*)&eModelType, szModelTypes, (_int)CModel::MODELTYPE_END);

	if (ImGuiFileDialog::Instance()->Display("Choose FBX"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			char	szRelativePath[MAX_PATH] = "";
			char	szDirectoryPath[MAX_PATH] = "";
			GetCurrentDirectoryA(MAX_PATH, szDirectoryPath);
			PathRelativePathToA(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY, ImGuiFileDialog::Instance()->GetFilePathName().c_str(), FILE_ATTRIBUTE_DIRECTORY);
			strcpy_s(szModelFilePath, sizeof(char) * MAX_PATH, szRelativePath);

			ImGuiFileDialog::Instance()->Close();
		}
		if (!ImGuiFileDialog::Instance()->IsOk())
			ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("Confirm"))
	{
		m_pModel = CModel::Create(m_pDevice, m_pContext, eModelType, szModelFilePath);
		m_pCloneModel = dynamic_cast<CModel*>(m_pModel->Clone(nullptr));
		sprintf_s(szModelFilePath, "");
		eModelType = CModel::MODELTYPE_END;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
	{
		sprintf_s(szModelFilePath, "");
		eModelType = CModel::MODELTYPE_END;
	}

	if (m_pModel != nullptr && m_pCloneModel != nullptr)
	{
		static char	szFileName[MAX_PATH] = "";

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Model Information"))
			m_pModel->ImGui_RenderProperty();

		ImGui::Separator();
		IMGUI_LEFT_LABEL(ImGui::InputText, "Save File Name", szFileName, MAX_PATH);
		
		if (ImGui::Button("Save by Binary"))
			ImGuiFileDialog::Instance()->OpenDialog("Select Folder", "Select Save Directory", ".model", "../Bin/Resource/Mesh", ".", 0, nullptr, ImGuiFileDialogFlags_Modal);
		ImGui::SameLine();
		if (ImGui::Button("Discard This Model"))
		{
			sprintf_s(szFileName, "");

			Safe_Release(m_pCloneModel);
			Safe_Release(m_pModel);
			return;
		}

		if (ImGuiFileDialog::Instance()->Display("Select Folder"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				char	szRelativePath[MAX_PATH] = "";
				char	szDirectoryPath[MAX_PATH] = "";
				GetCurrentDirectoryA(MAX_PATH, szDirectoryPath);
				PathRelativePathToA(szRelativePath, szDirectoryPath, FILE_ATTRIBUTE_DIRECTORY, ImGuiFileDialog::Instance()->GetCurrentPath().c_str(), FILE_ATTRIBUTE_DIRECTORY);
				strcat_s(szRelativePath, "\\");
				strcat_s(szRelativePath, szFileName);
				strcat_s(szRelativePath, ".model");

				m_pCloneModel->Save_Model(szRelativePath);

				ImGuiFileDialog::Instance()->Close();
				sprintf_s(szFileName, "");
			}
			if (!ImGuiFileDialog::Instance()->IsOk())
				ImGuiFileDialog::Instance()->Close();
		}
	}
}

CTool_ModelSave * CTool_ModelSave::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void * pArg)
{
	CTool_ModelSave*		pInstance = new CTool_ModelSave(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CTool_ModelSave");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_ModelSave::Free()
{
	Safe_Release(m_pCloneModel);
	Safe_Release(m_pModel);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}