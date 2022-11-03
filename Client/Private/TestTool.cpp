#include "stdafx.h"
#include "..\Public\TestTool.h"
#include "GameInstance.h"

CTestTool::CTestTool()
{
}

HRESULT CTestTool::Ready_Tool()
{
	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_Tool(this), E_FAIL);

	return S_OK;
}

HRESULT CTestTool::Update_Tool(_double dTimeDelta)
{
	static _bool	bTest = false;
	ImGui::Begin("Test Tool");

	// Test ImGuizmo Code
	ImGuizmo::BeginFrame();
	static ImGuizmo::OPERATION CurGuizmoType(ImGuizmo::TRANSLATE);

	ImGui::Text("This is Test Tool.");
	ImGui::SameLine();
	ImGui::Checkbox("Test CheckBox", &bTest);

	if (ImGui::Button("Test FileDialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseLoadMap", "Choose Folder", ".dat", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

	if (ImGuiFileDialog::Instance()->Display("ChooseLoadMap"))
	{
		if (!ImGuiFileDialog::Instance()->IsOk())
		{
			ImGuiFileDialog::Instance()->Close();
		}
	}

	ImGui::End();

	return S_OK;
}

CTestTool * CTestTool::Create()
{
	CTestTool*		pInstance = new CTestTool();

	if (FAILED(pInstance->Ready_Tool()))
	{
		MSG_BOX("Failed to Created : CTestTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestTool::Free()
{
}
