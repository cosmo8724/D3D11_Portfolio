#include "stdafx.h"
#include "..\Public\TestTool.h"
#include "GameInstance.h"
#include "ImCurveEdit.h"
#include "Camera.h"
#include "MySequencer.h"
#include "ImSequencer.h"

CTestTool::CTestTool()
{
	m_pSequencer = new CMySequencer;
	m_pSequencer->m_iFrameMax = 1000;
	m_pSequencer->m_iFrameMin = 0;
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 0, 10, 30, true });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 1, 20, 30, true });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 2, 12, 60, false });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 3, 61, 90, false });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 4, 90, 99, false });
}

HRESULT CTestTool::Initialize(void * pArg)
{
	m_szWIndowName = "Test Tool";

	FAILED_CHECK_RETURN(CGameInstance::GetInstance()->Add_ImGuiWindowObejct(this), E_FAIL);

	return S_OK;
}

void CTestTool::ImGui_RenderWindow()
{
	static _bool	bTest = false;
	ImGui::Begin("Test Tool");
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
	static _float&		fAlpha = ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w;
	ImGui::SliderFloat("Alpha", &fAlpha, 0.f, 1.f);
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


	/* Sequencer */
	/* TODO : ImGui를 동적할당해서 그런지 누수가 개쩜. 누수 잡기. */
	ImGui::Begin("Sequencer");

	static _int		iSelectedEntry = -1;
	static _int		iFristFrame = 0;
	static _bool		bExpanded = true;
	static _int		iCurrentFrame = 0;

	ImGui::PushItemWidth(130);
	ImGui::InputInt("Current Frame", &iCurrentFrame);
	iCurrentFrame++;
	ImGui::SameLine();
	ImGui::InputInt("Frame Min", &m_pSequencer->m_iFrameMin);
	ImGui::SameLine();
	ImGui::InputInt("Frame Max", &m_pSequencer->m_iFrameMax);

	ImGui::PopItemWidth();
	Sequencer(m_pSequencer, &iCurrentFrame, &bExpanded, &iSelectedEntry, &iFristFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);

	if (iSelectedEntry != -1)
	{
		const CMySequencer::ITEM&	Item = m_pSequencer->m_vecItems[iSelectedEntry];
		ImGui::Text("I am a %s, Please Edit me.", SequencerItemTypeNames[Item.m_iType]);
	}

	ImGui::End();

	return;
}

CTestTool * CTestTool::Create(void * pArg)
{
	CTestTool*		pInstance = new CTestTool();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTestTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestTool::Free()
{
	Safe_Release(m_pSequencer);
}
