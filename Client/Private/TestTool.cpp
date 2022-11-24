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
	ImGui::Text("This?");

	ImGui::Begin("ImGui Options");
	static _float&	fAlpha = ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w;
	IMGUI_LEFT_LABEL(ImGui::SliderFloat, "Alpha", &fAlpha, 0.f, 1.f);
	ImGui::Text("(CTRL+Click to Input Directly)");
	//ImGuiSliderFlags_
	ImGui::End();

	ImGui::ShowDemoWindow();

	ImGui::Begin("Debugger");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	/* Sequencer */
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
