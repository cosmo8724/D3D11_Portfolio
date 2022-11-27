#include "stdafx.h"
#include "..\Public\TestTool.h"
#include "GameInstance.h"
#include "ImCurveEdit.h"
#include "Camera.h"
#include "MySequencer.h"
#include "ImSequencer.h"
#include "GameUtility.h"

CTestTool::CTestTool()
{
	m_pSequencer = new CMySequencer;
	m_pSequencer->m_iFrameMax = 30;
	m_pSequencer->m_iFrameMin = 0;
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 0, 10, 30, true });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 1, 20, 30, true });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 2, 12, 30, false });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 3, 61, 30, false });
	m_pSequencer->m_vecItems.push_back(CMySequencer::ITEM{ 4, 90, 30, false });
}

HRESULT CTestTool::Initialize(void * pArg)
{
	m_szWIndowName = "Test Tool";

	return S_OK;
}

void CTestTool::ImGui_RenderWindow()
{
	ImGui::Text("This?");

	/* Sequencer */
	static _int		iSelectedEntry = -1;
	static _int		iFristFrame = 0;
	static _bool		bExpanded = true;
	static _int		iCurrentFrame = 0;
	static _bool		bNowPlaying = true;

	CGameUtility::Saturate(iCurrentFrame, m_pSequencer->m_iFrameMax, m_pSequencer->m_iFrameMin);
	if (bNowPlaying)
		iCurrentFrame++;

	ImGui::PushItemWidth(130);
	IMGUI_LEFT_LABEL(ImGui::InputInt, "Current Frame", &iCurrentFrame);
	ImGui::SameLine();
	if (ImGui::Button("Play/Pause"))
		bNowPlaying = !bNowPlaying;
	ImGui::SameLine();
	IMGUI_LEFT_LABEL(ImGui::InputInt, "Frame Min", &m_pSequencer->m_iFrameMin);
	ImGui::SameLine();
	IMGUI_LEFT_LABEL(ImGui::InputInt, "Frame Max", &m_pSequencer->m_iFrameMax);

	ImGui::PopItemWidth();
	Sequencer(m_pSequencer, &iCurrentFrame, &bExpanded, &iSelectedEntry, &iFristFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);

	if (iSelectedEntry != -1)
	{
		const CMySequencer::ITEM&	Item = m_pSequencer->m_vecItems[iSelectedEntry];
		ImGui::Text("I am a %s, Please Edit me.", SequencerItemTypeNames[Item.m_iType]);
	}

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
