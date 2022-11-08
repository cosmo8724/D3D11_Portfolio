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
	//ImGuizmo::BeginFrame();
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

	ImGui::Begin("Temp");
	ImGui::End();

	static bool show_demo_window = true;
	static bool show_another_window = false;

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	static float f = 0.0f;
	static int counter = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

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
