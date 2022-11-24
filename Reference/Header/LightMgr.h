#pragma once
#include "Base.h"

BEGIN(Engine)

class CLightMgr final : public CBase
{
	DECLARE_SINGLETON(CLightMgr)

private:
	CLightMgr();
	virtual ~CLightMgr() = default;

public:
	const LIGHTDESC*	Get_LightDesc(_uint iIndex);

public:
	HRESULT				Add_Light(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC& LightDesc);
	void					Clear();

private:
	vector<class CLight*>			m_vecLight;
	typedef vector<class CLight*>	LIGHTS;

public:
	virtual void			Free() override;
};

END