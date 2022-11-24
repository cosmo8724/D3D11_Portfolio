#include "stdafx.h"
#include "..\Public\CurveEditor.h"


CCurveEditor::CCurveEditor()
{
	m_vMyNode[0][0] = ImVec2(-10.f, 0);
	m_vMyNode[0][1] = ImVec2(20.f, 0.6f);
	m_vMyNode[0][2] = ImVec2(25.f, 0.2f);
	m_vMyNode[0][3] = ImVec2(70.f, 0.4f);
	m_vMyNode[0][4] = ImVec2(120.f, 1.f);
	m_iNodeCount[0] = 5;

	m_vMyNode[1][0] = ImVec2(-50.f, 0.2f);
	m_vMyNode[1][1] = ImVec2(33.f, 0.7f);
	m_vMyNode[1][2] = ImVec2(80.f, 0.2f);
	m_vMyNode[1][3] = ImVec2(82.f, 0.8f);
	m_vMyNode[1][4] = ImVec2(200.f, 0.34f);
	m_iNodeCount[1] = 5;
}

_int CCurveEditor::EditPoint(size_t CurveIndex, _int PointIndex, ImVec2 Value)
{
	m_vMyNode[CurveIndex][PointIndex] = ImVec2(Value.x, Value.y);
	SortValues(CurveIndex);

	for (size_t i = 0; i < GetPointCount(CurveIndex); ++i)
	{
		if (m_vMyNode[CurveIndex][i].x == Value.x)
			return (_int)i;
	}

	return PointIndex;
}

void CCurveEditor::AddPoint(size_t CurveIndex, ImVec2 Value)
{
	if (m_iNodeCount[CurveIndex] >= 8)
		return;

	m_vMyNode[CurveIndex][m_iNodeCount[CurveIndex]++] = Value;
	SortValues(CurveIndex);
}

void CCurveEditor::Free()
{
}