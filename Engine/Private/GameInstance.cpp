#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "ImGuiMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphicDev(CGraphic_Device::GetInstance())
	, m_pImGuiMgr(CImGuiMgr::GetInstance())
	, m_pInputDev(CInput_Device::GetInstance())
{
	Safe_AddRef(m_pGraphicDev);
	Safe_AddRef(m_pImGuiMgr);
	Safe_AddRef(m_pInputDev);
}

HRESULT CGameInstance::Initialize_Engine(const GRAPHIC_DESC & tGraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppContextOut)
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
	NULL_CHECK_RETURN(m_pInputDev, );
	NULL_CHECK_RETURN(m_pImGuiMgr, );

	m_pInputDev->Invalidate_Input_Device();

	m_pImGuiMgr->ImGui_NewFrame(dTimeDelta);
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

void CGameInstance::ImGui_Render()
{
	NULL_CHECK_RETURN(m_pImGuiMgr, );

	m_pImGuiMgr->ImGui_Render();
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

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	NULL_CHECK_RETURN(m_pInputDev, 0);

	return m_pInputDev->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState)
{
	NULL_CHECK_RETURN(m_pInputDev, 0);

	return m_pInputDev->Get_DIMouseMove(eMoveState);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CImGuiMgr::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pInputDev);
	Safe_Release(m_pImGuiMgr);
	Safe_Release(m_pGraphicDev);
}
