#include "stdafx.h"
#include "..\Public\MySequencer.h"
#include "imgui_internal.h"
#include "CurveEditor.h"

CMySequencer::CMySequencer()
{
	m_pCurveEditor = new CCurveEditor;
}

void CMySequencer::DoubleClick(_int Index)
{
	if (m_vecItems[Index].m_bExpended)
	{
		m_vecItems[Index].m_bExpended = false;
		return;
	}

	for (auto& Item : m_vecItems)
		Item.m_bExpended = false;

	m_vecItems[Index].m_bExpended = !m_vecItems[Index].m_bExpended;
}

void CMySequencer::CustomDraw(_int Index, ImDrawList * Draw_List, const ImRect & rc, const ImRect & LegendRect, const ImRect & ClippingRect, const ImRect & LegendClippingRect)
{
	m_pCurveEditor->m_vMax = ImVec2(_float(m_iFrameMax), 1.f);
	m_pCurveEditor->m_vMin = ImVec2(_float(m_iFrameMin), 0.f);

	Draw_List->PushClipRect(LegendClippingRect.Min, LegendClippingRect.Max, true);

	for (_int i = 0; i < 3; ++i)
	{
		ImVec2	pta(LegendRect.Min.x + 30, LegendRect.Min.y + i * 14.f);
		ImVec2	ptb(LegendRect.Max.x, LegendRect.Min.y + (i + 1) * 14.f);

		Draw_List->AddText(pta, m_pCurveEditor->m_bVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);

		if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
			m_pCurveEditor->m_bVisible[i] = !m_pCurveEditor->m_bVisible[i];
	}

	Draw_List->PopClipRect();

	ImGui::SetCursorScreenPos(rc.Min);
	ImCurveEdit::Edit(*m_pCurveEditor, rc.Max - rc.Min, 137 + Index, &ClippingRect);
}

void CMySequencer::CustomDrawCompact(_int Index, ImDrawList * Draw_List, const ImRect & rc, const ImRect & ClippingRect)
{
	m_pCurveEditor->m_vMax = ImVec2(_float(m_iFrameMax), 1.f);
	m_pCurveEditor->m_vMin = ImVec2(_float(m_iFrameMin), 0.f);

	Draw_List->PushClipRect(ClippingRect.Min, ClippingRect.Max, true);

	/* 2 == Curve Count */
	for (_int i = 0; i < 2; ++i)
	{
		for (_int j = 0; j < m_pCurveEditor->m_iNodeCount[i]; ++j)
		{
			_float		p = m_pCurveEditor->m_vMyNode[i][j].x;

			if (p < m_vecItems[Index].m_iFrameStart || p > m_vecItems[Index].m_iFrameEnd)
				continue;

			_float		r = (p - m_iFrameMin) / _float(m_iFrameMax - m_iFrameMin);
			_float		x = ImLerp(rc.Min.x, rc.Max.x, r);

			Draw_List->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
		}
	}
	
	Draw_List->PopClipRect();
}

void CMySequencer::Free()
{
	Safe_Release(m_pCurveEditor);
}