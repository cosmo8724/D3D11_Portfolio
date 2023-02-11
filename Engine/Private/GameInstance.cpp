#include "stdafx.h"
#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "ImGuiMgr.h"
#include "LevelMgr.h"
#include "ObjectMgr.h"
#include "TimerMgr.h"
#include "LightMgr.h"
#include "Frustum.h"
#include "RenderTargetMgr.h"
#include "FontMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

_uint				CGameInstance::m_iStaticLevelIndex = 0;
const wstring		CGameInstance::m_wstrPrototypeTransformTag = L"Prototype_Component_Transform";

CGameInstance::CGameInstance()
	: m_pGraphicDev(CGraphic_Device::GetInstance())
	, m_pImGuiMgr(CImGuiMgr::GetInstance())
	, m_pInputDev(CInput_Device::GetInstance())
	, m_pSoundMgr(CSoundMgr::GetInstance())
	, m_pLevelMgr(CLevelMgr::GetInstance())
	, m_pObjectMgr(CObjectMgr::GetInstance())
	, m_pComponentMgr(CComponentMgr::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pTimerMgr(CTimerMgr::GetInstance())
	, m_pLightMgr(CLightMgr::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pRenderTargetMgr(CRenderTargetMgr::GetInstance())
	, m_pFontMgr(CFontMgr::GetInstance())
{
	Safe_AddRef(m_pGraphicDev);
	Safe_AddRef(m_pImGuiMgr);
	Safe_AddRef(m_pInputDev);
	Safe_AddRef(m_pSoundMgr);
	Safe_AddRef(m_pLevelMgr);
	Safe_AddRef(m_pObjectMgr);
	Safe_AddRef(m_pComponentMgr);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimerMgr);
	Safe_AddRef(m_pLightMgr);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pRenderTargetMgr);
	Safe_AddRef(m_pFontMgr);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC & tGraphicDesc, DEVICE * ppDeviceOut, DEVICE_CONTEXT * ppContextOut)
{
	NULL_CHECK_RETURN(m_pGraphicDev, E_FAIL);
	NULL_CHECK_RETURN(m_pImGuiMgr, E_FAIL);
	NULL_CHECK_RETURN(m_pInputDev, E_FAIL);
	NULL_CHECK_RETURN(m_pSoundMgr, E_FAIL);
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);
	NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	m_hWnd = tGraphicDesc.hWnd;
	m_iStaticLevelIndex = iNumLevels;

	/* Initialize Graphic Device */
	FAILED_CHECK_RETURN(m_pGraphicDev->Ready_Graphic_Device(tGraphicDesc.hWnd, tGraphicDesc.eWindowMode, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut), E_FAIL);
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* Initialize InGui */
	FAILED_CHECK_RETURN(m_pImGuiMgr->Ready_ImGui(tGraphicDesc.hWnd, *ppDeviceOut, *ppContextOut), E_FAIL);

	/* Initialize Input Device */
	FAILED_CHECK_RETURN(m_pInputDev->Ready_Input_Device(tGraphicDesc.hInst, tGraphicDesc.hWnd), E_FAIL);

	FAILED_CHECK_RETURN(m_pObjectMgr->Reserve_Manager(iNumLevels + 1), E_FAIL);

	FAILED_CHECK_RETURN(m_pComponentMgr->Reserve_Manager(iNumLevels + 1), E_FAIL);

	/* Initialize FMOD */
	FAILED_CHECK_RETURN(m_pSoundMgr->Reserve_Manager("../Bin/Resource/Sound/"), E_FAIL);

	/* Create Prototype Transform Component */
	FAILED_CHECK_RETURN(m_pComponentMgr->Add_Prototype(m_iStaticLevelIndex, m_wstrPrototypeTransformTag, CTransform::Create(*ppDeviceOut, *ppContextOut)), E_FAIL);

	/* Initialize Frustum */
	FAILED_CHECK_RETURN(m_pFrustum->Initialize(), E_FAIL);

	/* Initialize RenderTarget */
	FAILED_CHECK_RETURN(m_pRenderTargetMgr->Initialize(*ppDeviceOut, *ppContextOut), E_FAIL);
	
	return S_OK;
}

void CGameInstance::Tick_Engine(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pLevelMgr, );
	NULL_CHECK_RETURN(m_pInputDev, );
	NULL_CHECK_RETURN(m_pImGuiMgr, );
	NULL_CHECK_RETURN(m_pObjectMgr, );
	NULL_CHECK_RETURN(m_pPipeLine, );
	NULL_CHECK_RETURN(m_pFrustum, );
	NULL_CHECK_RETURN(m_pRenderTargetMgr, );

	m_pInputDev->Invalidate_Input_Device();

	m_pObjectMgr->Tick(dTimeDelta);
	m_pLevelMgr->Tick(dTimeDelta);
	m_pPipeLine->Tick();

	m_pFrustum->TransformToWorldSpace();

	m_pObjectMgr->Late_Tick(dTimeDelta);
	m_pLevelMgr->Late_Tick(dTimeDelta);

	m_pRenderTargetMgr->Tick(dTimeDelta);

	m_pImGuiMgr->ImGui_NewFrame(dTimeDelta);

	m_pInputDev->Reset_EveryKey(dTimeDelta);
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

HRESULT CGameInstance::Add_ImGuiTabObject(CTool * pTool)
{
	NULL_CHECK_RETURN(m_pImGuiMgr, E_FAIL);

	return m_pImGuiMgr->Add_ImGuiTabObject(pTool);
}

HRESULT CGameInstance::Add_ImGuiWindowObject(CTool * pTool)
{
	NULL_CHECK_RETURN(m_pImGuiMgr, E_FAIL);

	return m_pImGuiMgr->Add_ImGuiWindowObject(pTool);
}

void CGameInstance::Clear_ImGuiObject()
{
	NULL_CHECK_RETURN(m_pImGuiMgr, );

	m_pImGuiMgr->Clear_ImGuiObject();
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

_double CGameInstance::Get_ChargeTime(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, 0.0);

	return m_pInputDev->Get_ChargeTime(byKeyID);
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

_bool CGameInstance::Mouse_Pressing(MOUSEKEYSTATE MouseButton)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Mouse_Pressing(MouseButton);
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_Pressing(byKeyID);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_Down(byKeyID);
}

_bool CGameInstance::Key_DoubleDown(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_DoubleDown(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_Up(byKeyID);
}

_bool CGameInstance::Key_Charge(_ubyte byKeyID, _double dTime)
{
	NULL_CHECK_RETURN(m_pInputDev, false);

	return m_pInputDev->Key_Charge(byKeyID, dTime);
}

void CGameInstance::Reset_EveryKey(_double dTimeDelta)
{
	NULL_CHECK_RETURN(m_pInputDev, );

	m_pInputDev->Reset_EveryKey(dTimeDelta);
}

void CGameInstance::Play_Sound(const wstring & pSoundKey, _float fVolume, _bool bIsBGM, _bool bRefresh, _int iManualChannelIndex)
{
	NULL_CHECK_RETURN(m_pSoundMgr, );

	m_pSoundMgr->Play_Sound(pSoundKey, fVolume, bIsBGM, bRefresh, iManualChannelIndex);
}

void CGameInstance::Stop_Sound(_uint iManualChannelIndex)
{
	NULL_CHECK_RETURN(m_pSoundMgr, );

	m_pSoundMgr->Stop_Sound(iManualChannelIndex);
}

void CGameInstance::Stop_All_Sound()
{
	NULL_CHECK_RETURN(m_pSoundMgr, );

	m_pSoundMgr->Stop_All();
}

void CGameInstance::Set_Volume(_uint iManualChannelIndex, _float fVolume)
{
	NULL_CHECK_RETURN(m_pSoundMgr, );

	m_pSoundMgr->Set_Volume(iManualChannelIndex, fVolume);
}

void CGameInstance::Set_MasterVolume(_float fVolume)
{
	NULL_CHECK_RETURN(m_pSoundMgr, );

	m_pSoundMgr->Set_MasterVolume(fVolume);
}

void CGameInstance::Set_SoundDesc(const wstring & wstrSoundKey, CSound::SOUND_DESC & SoundDesc)
{
	NULL_CHECK_RETURN(m_pSoundMgr, );

	m_pSoundMgr->Set_SoundDesc(wstrSoundKey, SoundDesc);
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

const _uint & CGameInstance::Get_CurLevelIndex()
{
	NULL_CHECK_RETURN(m_pLevelMgr, 1000);

	return m_pLevelMgr->Get_CurLevelIndex();
}

map<const wstring, class CGameObject*>* CGameInstance::Get_Prototypes()
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Get_Prototypes();
}

map<const wstring, class CLayer*>* CGameInstance::Get_Layers(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Get_Layers(iLevelIndex);
}

list<class CGameObject*>* CGameInstance::Get_CloneObjectList(_uint iLevelIndex, const wstring & wstrLayerTag)
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Get_CloneObjectList(iLevelIndex, wstrLayerTag);
}

vector<class CGameObject*>* CGameInstance::Get_AnimObjects()
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Get_AnimObjects();
}

HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const wstring & wstrLayerTag)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Add_Layer(iLevelIndex, wstrLayerTag);
}

HRESULT CGameInstance::Add_Prototype(const wstring & wstrPrototypeTag, CGameObject * pPrototype)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Add_Prototype(wstrPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_AnimObject(CGameObject * pAnimObject)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Add_AnimObject(pAnimObject);
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Clone_GameObject(iLevelIndex, wstrLayerTag, wstrPrototypeTag, pArg);
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, _float4x4 matWorld, void * pArg)
{
	NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	return m_pObjectMgr->Clone_GameObject(iLevelIndex, wstrLayerTag, wstrPrototypeTag, matWorld, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const wstring & wstrPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Clone_GameObject(wstrPrototypeTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, void * pArg)
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Clone_GameObjectReturnPtr(iLevelIndex, wstrLayerTag, wstrPrototypeTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring & wstrLayerTag, const wstring & wstrPrototypeTag, _float4x4 matWorld, void * pArg)
{
	NULL_CHECK_RETURN(m_pObjectMgr, nullptr);

	return m_pObjectMgr->Clone_GameObjectReturnPtr(iLevelIndex, wstrLayerTag, wstrPrototypeTag, matWorld, pArg);
}

map<const wstring, class CComponent*>* CGameInstance::Get_PrototypeComponents()
{
	NULL_CHECK_RETURN(m_pComponentMgr, nullptr);

	return m_pComponentMgr->Get_PrototypeComponents();
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & wstrPrototypeTag, CComponent * pPrototype)
{
	NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	return m_pComponentMgr->Add_Prototype(iLevelIndex, wstrPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Clear_Prototype(_uint iLevelIndex)
{
	NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	return m_pComponentMgr->Clear_Prototype(iLevelIndex);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & wstrPrototypeTag, CGameObject * pOwner, void * pArg)
{
	NULL_CHECK_RETURN(m_pComponentMgr, nullptr);
	
	return m_pComponentMgr->Clone_Component(iLevelIndex, wstrPrototypeTag, pOwner, pArg);
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

void CGameInstance::Set_ClipPlane(CPipeLine::CLIPPLANESTATE eState, const _float4 & vPlane)
{
	NULL_CHECK_RETURN(m_pPipeLine, );

	m_pPipeLine->Set_ClipPlane(eState, vPlane);
}

_float4 CGameInstance::Get_CameraPosition() const
{
	NULL_CHECK_RETURN(m_pPipeLine, _float4());

	return m_pPipeLine->Get_CameraPosition();
}

_float4 CGameInstance::Get_ClipPlane(CPipeLine::CLIPPLANESTATE eState) const
{
	NULL_CHECK_RETURN(m_pPipeLine, _float4());

	return m_pPipeLine->Get_ClipPlane(eState);
}

const _double CGameInstance::Get_TimeDelta(const wstring wstrTimerTag)
{
	NULL_CHECK_RETURN(m_pTimerMgr, 0.0);

	return m_pTimerMgr->Get_TimeDelta(wstrTimerTag);
}

void CGameInstance::Set_TimeScale(const wstring & wstrTimerTag, _double & dTimeDelta, _double dTimeScale)
{
	NULL_CHECK_RETURN(m_pTimerMgr, );

	return m_pTimerMgr->Set_TimeScale(wstrTimerTag, dTimeDelta, dTimeScale);
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

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	NULL_CHECK_RETURN(m_pLightMgr, nullptr);

	return m_pLightMgr->Get_LightDesc(iIndex);
}

_float4 & CGameInstance::Get_LightDiffuse(_uint iIndex)
{
	NULL_CHECK_RETURN(m_pLightMgr, _float4(0.f, 0.f, 0.f, 0.f));

	return m_pLightMgr->Get_LightDiffuse(iIndex);
}

void CGameInstance::Set_LightState(_uint iIndex, _bool bState)
{
	NULL_CHECK_RETURN(m_pLightMgr, );

	return m_pLightMgr->Set_LightState(iIndex, bState);
}

void CGameInstance::Set_LightPosition(_uint iIndex, _float4 vPos)
{
	NULL_CHECK_RETURN(m_pLightMgr, );

	return m_pLightMgr->Set_LightPosition(iIndex, vPos);
}

void CGameInstance::Set_LightRange(_uint iIndex, _float fRange)
{
	NULL_CHECK_RETURN(m_pLightMgr, );

	return m_pLightMgr->Set_LightRange(iIndex, fRange);
}

HRESULT CGameInstance::Add_Light(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC & LightDesc)
{
	NULL_CHECK_RETURN(m_pLightMgr, E_FAIL);

	return m_pLightMgr->Add_Light(pDevice, pContext, LightDesc);
}

_bool CGameInstance::IsInFrustum_World(_fvector vWorldPos, _float fRange)
{
	NULL_CHECK_RETURN(m_pFrustum, false);

	return m_pFrustum->IsInFrustum_World(vWorldPos, fRange);
}

_bool CGameInstance::IsInFrustum_Local(_fvector vLocalPos, _float fRange)
{
	NULL_CHECK_RETURN(m_pFrustum, false);

	return m_pFrustum->IsInFrustum_Local(vLocalPos, fRange);
}

HRESULT CGameInstance::Add_Font(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrFontTag, const wstring & wstrFontFilePath)
{
	NULL_CHECK_RETURN(m_pFontMgr, E_FAIL);

	return m_pFontMgr->Add_Font(pDevice, pContext, wstrFontTag, wstrFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring & wstrFontTag, const wstring & wstrText, const _float2 & vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	NULL_CHECK_RETURN(m_pFontMgr, E_FAIL);

	return m_pFontMgr->Render_Font(wstrFontTag, wstrText, vPos, fRadian, vScale, vColor);
}

ID3D11ShaderResourceView * CGameInstance::Get_ShaderResourceView(const wstring & wstrTargetTag)
{
	NULL_CHECK_RETURN(m_pRenderTargetMgr, nullptr);

	return m_pRenderTargetMgr->Get_ShaderResourceView(wstrTargetTag);
}

void CGameInstance::Release_Engine()
{
	CTimerMgr::GetInstance()->DestroyInstance();
	CRenderTargetMgr::GetInstance()->DestroyInstance();
	CGameInstance::GetInstance()->DestroyInstance();
	CImGuiMgr::GetInstance()->DestroyInstance();
	CObjectMgr::GetInstance()->DestroyInstance();
	CFrustum::GetInstance()->DestroyInstance();
	CFontMgr::GetInstance()->DestroyInstance();
	CPipeLine::GetInstance()->DestroyInstance();
	CComponentMgr::GetInstance()->DestroyInstance();
	CLevelMgr::GetInstance()->DestroyInstance();
	CSoundMgr::GetInstance()->DestroyInstance();
	CInput_Device::GetInstance()->DestroyInstance();
	CLightMgr::GetInstance()->DestroyInstance();
	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTimerMgr);
	Safe_Release(m_pImGuiMgr);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFontMgr);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponentMgr);
	Safe_Release(m_pLevelMgr);
	Safe_Release(m_pSoundMgr);
	Safe_Release(m_pInputDev);
	Safe_Release(m_pRenderTargetMgr);
	Safe_Release(m_pLightMgr);
	Safe_Release(m_pGraphicDev);
}