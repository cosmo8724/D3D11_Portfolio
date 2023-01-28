#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Engine)

class CGameInstance;
class CRenderer;

END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	Initialize();
	void		Tick(_double dTimeDelta);
	HRESULT	Render();

public:
	HRESULT	Resize_BackBuffer();

private:
	CGameInstance*		m_pGameInstance = nullptr;
	CRenderer*			m_pRenderer = nullptr;

	ID3D11Device*			m_pGraphicDev = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	HRESULT	Start_Level(LEVEL eLevel);
	HRESULT	Ready_Font();
	HRESULT	Ready_Prototype_Component();
	HRESULT	Ready_Prototype_GameObject();

public:
	static CMainApp*	Create();
	virtual void Free() override;
};

END