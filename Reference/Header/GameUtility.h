#pragma once
#include "Engine_Define.h"

class CGameUtility
{
public:
	CGameUtility() = default;
	~CGameUtility() = default;

public:
	static void wctc(const wchar_t* szWchar, char* szChar);
};

