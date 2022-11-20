#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "ImGuiMgr.h"
#include "LevelMgr.h"
#include "ObjectMgr.h"
#include "TimerMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

_uint				CGameInstance::m_iStaticLevelIndex = 0;
const wstring		CGameInstance::m_wstrPrototypeTransformTag = L"Prototype_Component_Transform";

CGameInstance::CGameInstance()
	: m_pGraphicDev(CGraphic_Device::GetInstance())
	, m_pImGuiMgr(CImGuiMgr::GetInstance())
	, m_pInputDev(CInput_Device::GetInstance())
	, m_pLevelMgr(CLevelMgr::GetInstance())
	, m_pObjectMgr(CObjectMgr::GetInstance())
	, m_pComponentMgr(CComponentMgr::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pTimerMgr(CTimerMgr::GetInstance())
{
	Safe_AddRef(m_pGraphicDev);
	Safe_AddRef(m_pImGuiMgr);
	Safe_AddRef(m_pInputDev);
	Safe_AddRef(m_pLevelMgr);
	Safe_AddRef(m_pObjectMgr);
	Safe_AddRef(m_pComponentMgr);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimerMgr);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC & tGraphicDesc, DEVICE * ppDeviceOut, DEVICE_CONTEXT * ppContextOut)
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);
	NULL_CHECK_RETURN(m_pImGuiMgr, E_FAIL);
	NULL_CHECK_RETURN(m_pInputDev, E_FAIL);
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);
	NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	m_iStaticLevelIndex = iNumLevels;

	/* Initialize Graphic Device */
	FAILED_CHECK_RETURN(m_pGraphicDev->Ready_Graphic_Device(tGraphicDesc.hWnd, tGraphicDesc.eWindowMode, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut), E_FAIL);

	/* Initialize InGui */
	FAILED_CHECK_RETURN(m_pImGuiMgr->Ready_ImGui(tGraphicDesc.hWnd, *ppDeviceOut, *ppContextOut), E_FAIL);

	/* Initialize Input Device */
	FAILED_CHECK_RETURN(m_pInputDev->Ready_Input_Device(tGraphicDesc.hInst, tGraphicDesc.hWnd), E_FAIL);

	FAILED_CHECK_RETURN(m_pObjectMgr->Reserve_Manager(iNumLevels + 1), E_FAIL);

	FAILED_CHECK_RETURN(m_pComponentMgr->Reserve_Manager(iNumLevels + 1), E_FAIL);

	/* Create Prototype Transform Component */
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_Prototype(m_iStaticLevelIndex, m_wstrPrototypeTransformTag, CTransform::Create(*ppDeviceOut, *ppContextOut)), E_FAIL);
	
	return S_OK;
}

void CGameInstance::Tick_Engine(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pLevelMgr, );
	NULL_CHECK_RETURN(m_pInputDev, );
	NULL_CHECK_RETURN(m_pImGuiMgr, );
	NULL_CHECK_RETURN(m_pObjectMgr, );

	m_pInputDev->Invalidate_Input_Device();

	m_pObjectMgr->Tick(dTimeDelta);
	m_pLevelMgr->Tick(dTimeDelta);
	m_pPipeLine->Tick();

	m_pObjectMgr->Late_Tick(dTimeDelta);
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

HRESULT CGameInstance::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate)
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);

	return m_pGraphicDev->Update_SwapChain(hWnd, iWinCX, iWinCY, bIsFullScreen, bNeedUpdate);
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

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & wstrPrototypeTag, CComponent * pPrototype)
{
	NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	return m_pComponentMgr->Add_Prototype(iLevelIndex, wstrPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & wstrPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pComponentMgr, nullptr);
	
	return m_pComponentMgr->Clone_Component(iLevelIndex, wstrPrototypeTag, pArg);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, XMMatrixIdentity());

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, XMMatrixIdentity());

	return m_pPipeLine->Get_TransformMatrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	NULL_CHECK_RETURN(m_pPipeLine, _float4x4());

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	NULL_CHECK_RETURN(m_pPipeLine, );

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

const _double CGameInstance::Get_TimeDelta(const wstring wstrTimerTag)
{
	NULL_CHECK_RETURN(m_pTimerMgr, 0.0);

	return m_pTimerMgr->Get_TimeDelta(wstrTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const wstring wstrTimerTag)
{
	NULL_CHECK_RETURN(m_pTimerMgr, E_FAIL);
	
	return m_pTimerMgr->Ready_Timer(wstrTimerTag);
}

void CGameInstance::Update_Timer(const wstring wstrTimerTag)
{
	NULL_CHECK_RETURN(m_pTimerMgr, );

	m_pTimerMgr->Update_Timer(wstrTimerTag);
}

void CGameInstance::Release_Engine()
{
	CTimerMgr::GetInstance()->DestroyInstance();
	CGameInstance::GetInstance()->DestroyInstance();
	CObjectMgr::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CComponentMgr::GetInstance()->DestroyInstance();
	CLevelMgr::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CImGuiMgr::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTimerMgr);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponentMgr);
	Safe_Release(m_pLevelMgr);
	Safe_Release(m_pInputDev);
	Safe_Release(m_pImGuiMgr);
	Safe_Release(m_pGraphicDev);
}
