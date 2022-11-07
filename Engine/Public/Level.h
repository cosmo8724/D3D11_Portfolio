#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double dTimeDelta);
	virtual void Late_Tick(_double dTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

public:
	virtual void Free();
};

END