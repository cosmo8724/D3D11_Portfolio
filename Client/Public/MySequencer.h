#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "ImSequencer.h"

BEGIN(Client)

static const char* SequencerItemTypeNames[] = { "Camera", "Music", "ScreenEffect", "FadeIn", "Animation" };
static const char* labels[] = { "Translation", "Rotation" , "Scale" };

class CMySequencer : public ImSequencer::SequenceInterface, public CBase
{
public:
	typedef struct MySequenceItem
	{
		_int	m_iType;			// One of SuquencerItemTypeNames
		_int	m_iFrameStart;
		_int	m_iFrameEnd;
		_bool	m_bExpended;		// Use CurveEditor
	} ITEM;

public:
	CMySequencer();
	virtual	~CMySequencer() = default;

public:
	virtual _int			GetFrameMin() const override { return m_iFrameMin; }
	virtual _int			GetFrameMax() const override { return m_iFrameMax; }
	virtual _int			GetItemCount() const override { return (_int)m_vecItems.size(); }
	virtual _int			GetItemTypeCount() const override { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
	virtual const char*	GetItemTypeName(_int TypeIndex) const override {	return SequencerItemTypeNames[TypeIndex];	}
	virtual const char*	GetItemLabel(_int Index) const override {
		static char tmps[512];
		snprintf(tmps, 512, "[%02d] %s", Index, SequencerItemTypeNames[m_vecItems[Index].m_iType]);
		return tmps;
	}
	virtual size_t			GetCustomHeight(_int Index) override { return m_vecItems[Index].m_bExpended ? 300 : 0; }
	virtual void			Get(_int Index, _int** Start, _int** End, _int* Type, _uint* Color) override {
		MySequenceItem& Item = m_vecItems[Index];
		if (Color)
			*Color = 0xFFAA8080;
		if (Start)
			*Start = &Item.m_iFrameStart;
		if (End)
			*End = &Item.m_iFrameEnd;
		if (Type)
			*Type = Item.m_iType;
	}
	
public:
	virtual void			Add(_int Type) override { m_vecItems.push_back(ITEM{ Type, 0, 10, false }); }
	virtual void			Del(_int Index) override { m_vecItems.erase(m_vecItems.begin() + Index); }
	virtual void			Duplicate(_int Index) override { m_vecItems.push_back(m_vecItems[Index]); }

public:
	_int						m_iFrameMin = 0;
	_int						m_iFrameMax = 0;
	vector<ITEM>				m_vecItems;
	class CCurveEditor*		m_pCurveEditor = nullptr;

private:
	virtual void			DoubleClick(_int Index) override;
	virtual void			CustomDraw(_int Index, ImDrawList* Draw_List, const ImRect& rc, const ImRect& LegendRect, const ImRect& ClippingRect, const ImRect& LegendClippingRect) override;
	virtual void			CustomDrawCompact(_int Index, ImDrawList* Draw_List, const ImRect& rc, const ImRect& ClippingRect) override;

public:
	virtual void			Free() override;
};

END