#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT		Initialize() override;
	virtual void			Tick(_double dTimeDelta) override;
	virtual void			Late_Tick(_double dTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	static CLevel_Logo*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual void			Free() override;
};

END