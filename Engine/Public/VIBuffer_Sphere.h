#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
private:
	CVIBuffer_Sphere(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs);
	virtual ~CVIBuffer_Sphere() = default;

public:
	HRESULT						Initialize_Prototype();
	virtual HRESULT				Initialize(void* pArg) override;

private:
	_uint							m_iSegments = 0;
	_uint							m_iSlices = 0;

public:
	static	CVIBuffer_Sphere*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CComponent*		Clone(void* pArg = nullptr) override;
	virtual void					Free() override;
};

END