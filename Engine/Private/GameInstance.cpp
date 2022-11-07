#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "ImGuiMgr.h"
#include "LevelMgr.h"
#include "ObjectMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphicDev(CGraphic_Device::GetInstance())
	, m_pImGuiMgr(CImGuiMgr::GetInstance())
	, m_pInputDev(CInput_Device::GetInstance())
	, m_pLevelMgr(CLevelMgr::GetInstance())
	, m_pObjectMgr(CObjectMgr::GetInstance())
{
	Safe_AddRef(m_pGraphicDev);
	Safe_AddRef(m_pImGuiMgr);
	Safe_AddRef(m_pInputDev);
	Safe_AddRef(m_pLevelMgr);
	Safe_AddRef(m_pObjectMgr);
}

HRESULT CGameInstance::Initialize_Engine(const GRAPHIC_DESC & tGraphicDesc, DEVICE * ppDeviceOut, DEVICE_CONTEXT * ppContextOut)
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);

	/* Initialize Graphic Device */
	FAILED_CHECK_RETURN(m_pGraphicDev->Ready_Graphic_Device(tGraphicDesc.hWnd, tGraphicDesc.eWindowMode, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut), E_FAIL);

	/* Initialize InGui */
	FAILED_CHECK_RETURN(m_pImGuiMgr->Ready_ImGui(tGraphicDesc.hWnd, *ppDeviceOut, *ppContextOut), E_FAIL);

	/* Initialize Input Device */
	FAILED_CHECK_RETURN(m_pInputDev->Ready_Input_Device(tGraphicDesc.hInst, tGraphicDesc.hWnd), E_FAIL);
	
	return S_OK;
}

void CGameInstance::Tick_Engine(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pLevelMgr, );
	NULL_CHECK_RETURN(m_pInputDev, );
	NULL_CHECK_RETURN(m_pImGuiMgr, );

	m_pInputDev->Invalidate_Input_Device();

	m_pLevelMgr->Tick(dTimeDelta);
	m_pLevelMgr->Late_Tick(dTimeDelta);
	m_pInputDev->Reset_EveryKey();

	m_pImGuiMgr->ImGui_NewFrame(dTimeDelta);
}

void CGameInstance::Clear_Level(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pObjectMgr, );

	m_pObjectMgr->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_Graphic_Device(const _float4 * pColor)
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);

	FAILED_CHECK_RETURN(m_pGraphicDev->Clear_BackBuffer_View(*pColor), E_FAIL);
	FAILED_CHECK_RETURN(m_pGraphicDev->Clear_DepthStencil_View(), E_FAIL);

	return S_OK;
}

HRESULT CGameInstance::Present()
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);

	return m_pGraphicDev->Present();
}

HRESULT CGameInstance::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY)
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);

	return m_pGraphicDev->Update_SwapChain(hWnd, iWinCX, iWinCY);
}

void CGameInstance::ImGui_Render()
{
	NULL_CHECK_RETURN(m_pImGuiMgr, );

	m_pImGuiMgr->ImGui_Render();
}

void CGameInstance::ImGui_Render_Update()
{
	NULL_CHECK_RETURN(m_pImGuiMgr, );

	m_pImGuiMgr->ImGui_Render_Update();
}

HRESULT CGameInstance::Add_Tool(CTool * pTool)
{
	NULL_CHECK_RETURN(m_pImGuiMgr, E_FAIL);

	return m_pImGuiMgr->Add_Tool(pTool);
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, 0);

	return m_pInputDev->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE byMouseID)
{
	NULL_CHECK_RETURN(m_pInputDev, 0);

	return m_pInputDev->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
{
	NULL_CHECK_RETURN(m_pInputDev, 0);

	return m_pInputDev->Get_DIMouseMove(eMoveState);
}

_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE MouseButton)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Mouse_Down(MouseButton);
}

_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE MouseButton)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Mouse_Up(MouseButton);
}

_bool CGameInstance::Mouse_DoubleClick(MOUSEKEYSTATE MouseButton)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Mouse_DoubleClick(MouseButton);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_Up(byKeyID);
}

void CGameInstance::Reset_EveryKey()
{
	NULL_CHECK_RETURN(m_pInputDev, );

	m_pInputDev->Reset_EveryKey();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	NULL_CHECK_RETURN(m_pLevelMgr, E_FAIL);

	return m_pLevelMgr->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	NULL_CHECK_RETURN(m_pLevelMgr, E_FAIL);

	return m_pLevelMgr->Render();
}

HRESULT CGameInstance::Add_Prototype(const wstring & wstrPrototypeTag, CGameObject * pPrototype)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Add_Prototype(wstrPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Clone_GameObject(iLevelIndex, wstrLayerTag, wstrPrototypeTag, pArg);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CObjectMgr::GetInstance()->DestroyInstance();
	CLevelMgr::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CImGuiMgr::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pLevelMgr);
	Safe_Release(m_pInputDev);
	Safe_Release(m_pImGuiMgr);
	Safe_Release(m_pGraphicDev);
}
