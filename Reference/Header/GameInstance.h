#pragma once
#include "Base.h"
#include "Input_Device.h"

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

public:		/* For InputDevice */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState);

private:
	class CGraphic_Device*		m_pGraphicDev = nullptr;
	class CInput_Device*		m_pInputDev = nullptr;

public:
	static void	Release_Engine();

public:
	virtual void Free() override;
};

END