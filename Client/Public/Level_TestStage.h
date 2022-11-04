#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_TestStage final : public CLevel
{
private:
	CLevel_TestStage(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLevel_TestStage() = default;

public:
	virtual HRESULT				Initialize() override;
	virtual void					Tick(_double dTimeDelta) override;
	virtual void					Late_Tick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	static CLevel_TestStage*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual void					Free() override;
};

END