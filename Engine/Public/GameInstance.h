#pragma once
#include "Base.h"
#include "Input_Device.h"
#include "ComponentMgr.h"
#include "Tool.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:		/* For GameInstance */
	HRESULT	Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& tGraphicDesc, DEVICE* ppDeviceOut, DEVICE_CONTEXT* ppContextOut);
	void		Tick_Engine(_double dTimeDelta);
	void		Clear_Level(_uint iLevelIndex);

public:		/* For GraphicDevice */
	HRESULT	Clear_Graphic_Device(const _float4* pColor);
	HRESULT	Present();
	HRESULT	Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

public:		/* For ImGui */
	void		ImGui_Render();
	void		ImGui_Render_Update();
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
	HRESULT	Open_Level(_uint iLevelIndelx, class CLevel* pNewLevel);
	HRESULT	Render_Level();

public:		/* For Object Manager */
	HRESULT	Add_Prototype(const wstring& wstrPrototypeTag, class CGameObject* pPrototype);
	HRESULT	Clone_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, void* pArg = nullptr);

public:		/* For Component Manager */
	HRESULT				Add_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag, class CComponent* pPrototype);
	class CComponent*	Clone_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg = nullptr);

private:
	class CGraphic_Device*		m_pGraphicDev = nullptr;
	class CImGuiMgr*			m_pImGuiMgr = nullptr;
	class CInput_Device*		m_pInputDev = nullptr;
	class CLevelMgr*				m_pLevelMgr = nullptr;
	class CObjectMgr*			m_pObjectMgr = nullptr;
	class CComponentMgr*		m_pComponentMgr = nullptr;

public:
	static void	Release_Engine();

public:
	virtual void Free() override;
};

END