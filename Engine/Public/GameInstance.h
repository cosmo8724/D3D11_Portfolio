#pragma once
#include "Base.h"
#include "Input_Device.h"
#include "Tool.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:		/* For GameInstance */
	HRESULT	Initialize_Engine(const GRAPHIC_DESC& tGraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void		Tick_Engine(_double dTimeDelta);

public:		/* For GraphicDevice */
	HRESULT	Clear_Graphic_Device(const _float4* pColor);
	HRESULT	Present();

public:		/* For ImGui */
	void		ImGui_Render();
	HRESULT	Add_Tool(CTool* pTool);

public:		/* For InputDevice */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
	_bool		Mouse_Down(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_Up(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_DoubleClick(MOUSEKEYSTATE MouseButton);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);
	void		Reset_EveryKey();

public:		/* For Level Manager */
	HRESULT	Open_Level(class CLevel* pNewLevel);
	HRESULT	Render_Level();

private:
	class CGraphic_Device*		m_pGraphicDev = nullptr;
	class CImGuiMgr*			m_pImGuiMgr = nullptr;
	class CInput_Device*		m_pInputDev = nullptr;
	class CLevelMgr*				m_pLevelMgr = nullptr;

public:
	static void	Release_Engine();

public:
	virtual void Free() override;
};

END