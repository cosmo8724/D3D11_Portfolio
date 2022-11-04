#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT	Initialize(LEVEL eNextLevel);	/* Only For LoadingLevel */
	virtual void		Tick(_double dTimeDelta) override;
	virtual void		Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render() override;

private:
	class CLoader*	m_pLoader = nullptr;
	LEVEL				m_eNextLevel = LEVEL_END;

public:
	static CLevel_Loading*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, LEVEL eNextLevel);
	virtual void					Free() override;
};

END