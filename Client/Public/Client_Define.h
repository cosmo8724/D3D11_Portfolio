#pragma once
#include <process.h>
#include <ctime>
#include "GameObject.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

namespace Engine
{
	class CBone;
	class CTransform;
}

namespace Client
{
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_TESTSTAGE, LEVEL_END };
	enum NPC { NPC_ANDY, NPC_BAINE, NPC_GRANDMA, NPC_KLEMENS_KLOUT, NPC_CORDELIA_KLOUT, SIGRID, NPC_END };
	class CCursor;

	typedef  struct tagHatDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float4x4		matSocketPivot;
		CBone*		pSocket;
		CTransform*	pPlayerTransformCom;
		class CSigrid*	pPlayer;
	} HATDESC;
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;
extern Client::CCursor*		g_pCursor;
extern _bool			g_bReadySceneChange;
extern _bool			g_bShopOpen;
extern _bool			g_bEnd;
extern _float			g_fSFXVolume;
extern _float			g_fVoiceVolume;
extern _float			g_fBossVolume;
extern _float			g_fEnvironmentVolume;

using namespace Client;