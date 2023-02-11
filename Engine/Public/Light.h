#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
public:
	CLight(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLight() = default;

public:
	const LIGHTDESC*	Get_LightDesc() const { return &m_LightDesc; }
	_float4&				Get_Diffuse() { return m_LightDesc.vDiffuse; }
	void					Set_LightState(_bool bState) { m_LightDesc.bIsLightOn = bState; }
	void					Set_LightPosition(_float4 vPos) { m_LightDesc.vPosition = vPos; }
	void					Set_LightRange(_float fRange) { m_LightDesc.fRange = fRange; }

public:
	HRESULT				Initialize(const LIGHTDESC& LightDesc);
	HRESULT				Render(class CVIBuffer_Rect* pVIBufferCom, class CShader* pShaderCom);

private:
	DEVICE				m_pDevice = nullptr;
	DEVICE_CONTEXT	m_pContext = nullptr;

	LIGHTDESC			m_LightDesc;

public:
	static CLight*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const LIGHTDESC& LightDesc);
	virtual void			Free() override;
};

END