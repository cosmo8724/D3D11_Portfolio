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
	static string		WstrToStr(const wstring& wstrSour);
	static void		SplitTag(wchar_t* wszTag, wstring& wszChar);	// Split Tag by '_'
	static void		ConvertNumberTo_d(char* szChar);
	static COMPONENTTYPE	CheckComponentTypeFromTag(const wstring& wstrComponentTag);

	static void		Saturate(int& InValue, int InMax, int InMin);
	static void		DecomposePitchYawRollFromXMMatrix(_fmatrix matRotation, _float& fPitch, _float& fYaw, _float& fRoll);
};

