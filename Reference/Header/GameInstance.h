#pragma once
#include "Base.h"
#include "Input_Device.h"
#include "SoundMgr.h"
#include "ComponentMgr.h"
#include "PipeLine.h"
#include "Tool.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HWND					Get_Handle() { return m_hWnd; }
	static _uint			Get_StaticLevelIndex() { return m_iStaticLevelIndex; }

public:
	static const wstring		m_wstrPrototypeTransformTag;

public:		/* For GameInstance */
	HRESULT				Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& tGraphicDesc, DEVICE* ppDeviceOut, DEVICE_CONTEXT* ppContextOut);
	void					Tick_Engine(_double dTimeDelta);
	void					Clear_Level(_uint iLevelIndex);

public:		/* For GraphicDevice */
	HRESULT				Clear_Graphic_Device(const _float4* pColor);
	HRESULT				Present();
	HRESULT				Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

public:		/* For ImGui */
	void					ImGui_Render();
	void					ImGui_Render_Update();
	HRESULT				Add_ImGuiTabObject(class CTool* pTool);
	HRESULT				Add_ImGuiWindowObject(class CTool* pTool);
	void					Clear_ImGuiObject();

public:		/* For InputDevice */
	_byte					Get_DIKeyState(_ubyte byKeyID);
	_byte					Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long					Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
	_double				Get_ChargeTime(_ubyte byKeyID);
	_bool					Mouse_Down(MOUSEKEYSTATE MouseButton);
	_bool					Mouse_Up(MOUSEKEYSTATE MouseButton);
	_bool					Mouse_DoubleClick(MOUSEKEYSTATE MouseButton);
	_bool					Mouse_Pressing(MOUSEKEYSTATE MouseButton);
	_bool					Key_Pressing(_ubyte byKeyID);
	_bool					Key_Down(_ubyte byKeyID);
	_bool					Key_DoubleDown(_ubyte byKeyID);
	_bool					Key_Up(_ubyte byKeyID);
	_bool					Key_Charge(_ubyte byKeyID, _double dTime);
	void					Reset_EveryKey(_double dTimeDelta);

public:		/* For Sound Manager */
	void					Play_Sound(const wstring& pSoundKey, _float fVolume, _bool bIsBGM = false, _bool bRefresh = false, _int iManualChannelIndex = -1);
	void					Stop_Sound(_uint iManualChannelIndex);
	void					Stop_All_Sound();
	void					Set_Volume(_uint iManualChannelIndex, _float fVolume);
	void					Set_MasterVolume(_float fVolume);
	void					Set_SoundDesc(const wstring& wstrSoundKey, CSound::SOUND_DESC& SoundDesc);

public:		/* For Level Manager */
	HRESULT				Open_Level(_uint iLevelIndelx, class CLevel* pNewLevel);
	HRESULT				Render_Level();
	const _uint&			Get_CurLevelIndex();

public:		/* For Object Manager */
	map<const wstring, class CGameObject*>*	Get_Prototypes();
	map<const wstring, class CLayer*>*			Get_Layers(_uint iLevelIndex);
	list<class CGameObject*>*						Get_CloneObjectList(_uint iLevelIndex, const wstring& wstrLayerTag);
	vector<class CGameObject*>*					Get_AnimObjects();
	HRESULT				Add_Layer(_uint iLevelIndex, const wstring& wstrLayerTag);
	HRESULT				Add_Prototype(const wstring& wstrPrototypeTag, class CGameObject* pPrototype);
	HRESULT				Add_AnimObject(class CGameObject* pAnimObject);
	HRESULT				Clone_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, void* pArg = nullptr);
	HRESULT				Clone_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, _float4x4 matWorld, void* pArg = nullptr);
	class CGameObject*		Clone_GameObject(const wstring& wstrPrototypeTag, void* pArg = nullptr);
	class CGameObject*		Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, void* pArg = nullptr);
	class CGameObject*		Clone_GameObjectReturnPtr(_uint iLevelIndex, const wstring& wstrLayerTag, const wstring& wstrPrototypeTag, _float4x4 matWorld, void* pArg = nullptr);

public:		/* For Component Manager */
	map<const wstring, class CComponent*>*		Get_PrototypeComponents();
	HRESULT				Add_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag, class CComponent* pPrototype);
	HRESULT				Clear_Prototype(_uint iLevelIndex);
	class CComponent*	Clone_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, class CGameObject* pOwner, void* pArg = nullptr);

public:		/* For PipeLine */
	_matrix				Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_matrix				Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState);
	_float4x4				Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState);
	void					Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4				Get_CameraPosition() const;

public:		/* For Timer Manager */
	const _double			Get_TimeDelta(const wstring wstrTimerTag);
	void					Set_TimeScale(const wstring& wstrTimerTag, _double& dTimeDelta, _double dTimeScale);
	HRESULT				Ready_Timer(const wstring wstrTimerTag);
	void					Update_Timer(const wstring wstrTimerTag);

public:		/* For Light Manager */
	const LIGHTDESC*	Get_LightDesc(_uint iIndex);
	HRESULT				Add_Light(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC& LightDesc);

public:		/* For Frustum */
	_bool					IsInFrustum_World(_fvector vWorldPos, _float fRange);
	_bool					IsInFrustum_Local(_fvector vLocalPos, _float fRange);

public:		/* For Font Manager */
	HRESULT				Add_Font(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrFontTag, const wstring& wstrFontFilePath);
	HRESULT				Render_Font(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));

private:
	HWND							m_hWnd;

private:
	static _uint					m_iStaticLevelIndex;

private:
	class CGraphic_Device*		m_pGraphicDev = nullptr;
	class CImGuiMgr*			m_pImGuiMgr = nullptr;
	class CInput_Device*		m_pInputDev = nullptr;
	class CSoundMgr*			m_pSoundMgr = nullptr;
	class CLevelMgr*				m_pLevelMgr = nullptr;
	class CObjectMgr*			m_pObjectMgr = nullptr;
	class CComponentMgr*		m_pComponentMgr = nullptr;
	class CPipeLine*				m_pPipeLine = nullptr;
	class CTimerMgr*			m_pTimerMgr = nullptr;
	class CLightMgr*				m_pLightMgr = nullptr;
	class CFrustum*				m_pFrustum = nullptr;
	class CRenderTargetMgr*	m_pRenderTargetMgr = nullptr;
	class CFontMgr*				m_pFontMgr = nullptr;

public:
	static void	Release_Engine();

public:
	virtual void Free() override;
};

END