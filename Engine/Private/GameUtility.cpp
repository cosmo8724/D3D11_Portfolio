#include "..\Public\GameUtility.h"

void CGameUtility::wctc(const wchar_t* szWchar, char* szChar)
{
	_int iLength;
	_int slength = lstrlen(szWchar) + 1;
	iLength = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
	::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, iLength, 0, 0);
}

void CGameUtility::ctwc(const char * szChar, wchar_t * wszChar)
{
	_int iLength = strlen(szChar) + 1;
	_int slength;
	slength = ::MultiByteToWideChar(CP_ACP, 0, szChar, -1, 0, 0);
	::MultiByteToWideChar(CP_ACP, MB_COMPOSITE, szChar, iLength, wszChar, slength);
}

void CGameUtility::Saturate(int & InValue, int InMax, int InMin)
{
	if (InValue > InMax)
		InValue = InMin;
	if (InValue < InMin)
		InValue = InMax;
}
