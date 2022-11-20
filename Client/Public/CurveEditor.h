#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "ImCurveEdit.h"

BEGIN(Client)

class CCurveEditor : public ImCurveEdit::Delegate, public CBase
{
public:
	CCurveEditor();
	virtual ~CCurveEditor() = default;

public:
	virtual ImVec2*	GetPoints(size_t CurveIndex) { return  m_vMyNode[CurveIndex]; }
	virtual size_t		GetCurveCount() override  { return 2; }
	virtual size_t		GetPointCount(size_t CurveIndex) override { return m_iNodeCount[CurveIndex]; } 
	virtual uint32_t	GetCurveColor(size_t CurveIndex) { return Cols[CurveIndex]; }
	virtual ImCurveEdit::CurveType	GetCurveType(size_t CurveIndex) const override { return ImCurveEdit::CurveSmooth; }
	virtual ImVec2&	GetMax() override { return m_vMax; }
	virtual ImVec2&	GetMin() override { return m_vMin; }
	virtual _uint		GetBackgroundColor() { return 0; }
	virtual _bool		IsVisible(size_t CurveIndex) override { return m_bVisible[CurveIndex]; }

public:
	virtual _int		EditPoint(size_t CurveIndex, _int PointIndex, ImVec2 Value) override;
	virtual void		AddPoint(size_t CurveIndex, ImVec2 Value) override;

public:
	ImVec2	m_vMyNode[2][5];
	size_t		m_iNodeCount[2];
	_bool		m_bVisible[2];
	ImVec2	m_vMin;
	ImVec2	m_vMax;
	uint32_t	Cols[2] = { 0Xff0000ff, 0xff00ff00 };

private:
	void				SortValues(size_t CurveIndex)
	{
		auto	Begin = std::begin(m_vMyNode[CurveIndex]);
		auto	End = std::begin(m_vMyNode[CurveIndex]) + GetPointCount(CurveIndex);
		std::sort(Begin, End, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });
	}

public:
	virtual void		Free()	override;
};

END