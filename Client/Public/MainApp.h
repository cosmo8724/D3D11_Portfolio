#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

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

private:
	CGameInstance*		m_pGameInstance = nullptr;
	ID3D11Device*			m_pGraphicDev = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

public:
	static CMainApp*	Create();
	virtual void Free() override;
};

END