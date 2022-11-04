#pragma once
#include "Base.h"

BEGIN(Engine)

class CLevelMgr final : public CBase
{
	DECLARE_SINGLETON(CLevelMgr)

private:
	CLevelMgr();
	virtual ~CLevelMgr() = default;

public:
	HRESULT		Open_Level(class CLevel* pNewLevel);
	void			Tick(_double dTimeDelta);
	void			Late_Tick(_double dTimeDelta);
	HRESULT		Render();

private:
	class CLevel*		m_pCurrentLevel = nullptr;

public:
	virtual void	Free() override;
};

END