#pragma once
#include "Engine_Define.h"

class ENGINE_DLL CGameUtility
{
public:
	CGameUtility() = default;
	~CGameUtility() = default;

public:
	static void		wctc(const wchar_t* szWchar, char* szChar);
	static void		ctwc(const char* szChar, wchar_t* wszChar);
	static void		SplitTag(wchar_t* wszTag, wstring& wszChar);
	static void		Saturate(int& InValue, int InMax, int InMin);
};

