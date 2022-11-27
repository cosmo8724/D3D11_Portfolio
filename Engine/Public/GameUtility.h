#pragma once
#include "Engine_Define.h"

class ENGINE_DLL CGameUtility
{
public:
	CGameUtility() = default;
	~CGameUtility() = default;

public:
	static void wctc(const wchar_t* szWchar, char* szChar);
	static void Saturate(int& InValue, int InMax, int InMin);
};

