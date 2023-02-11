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
	_float4&				Get_LightDiffuse(_uint iIndex);
	void					Set_LightState(_uint iIndex, _bool bState);
	void					Set_LightPosition(_uint iIndex, _float4 vPos);
	void					Set_LightRange(_uint iIndex, _float fRange);

public:
	HRESULT				Add_Light(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC& LightDesc);
	void					Render_Light(class CVIBuffer_Rect* pVIBufferCom, class CShader* pShaderCom);
	void					Clear();

private:
	vector<class CLight*>			m_vecLight;
	typedef vector<class CLight*>	LIGHTS;

public:
	virtual void			Free() override;
};

END