#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORMSTATE { D3DTS_CAMWORLD, D3DTS_VIEW, D3DTS_REFLECTVIEW, D3DTS_PROJ, D3DTS_END };
	enum CLIPPLANESTATE { CLIPPLANE_REFLECT, CLIPPLANE_REFRACT, CLIPPLANE_END};

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix		Get_TransformMatrix(TRANSFORMSTATE eState) const;
	_matrix		Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const;
	_float4x4		Get_TransformFloat4x4(TRANSFORMSTATE eState) const;
	_float4x4		Get_TransformFloat4x4_Inverse(TRANSFORMSTATE eState) const;
	_float4		Get_CameraPosition() const { return m_vCameraPosition; }
	_float4		Get_ClipPlane(CLIPPLANESTATE eState) const { return m_vClipPlane[eState]; }

public:
	void			Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void			Set_ClipPlane(CLIPPLANESTATE eState, const _float4& vPlane) { m_vClipPlane[eState] = vPlane; }

public:
	void			Tick();

private:
	_float4x4		m_TransformMatrix[D3DTS_END];
	_float4x4		m_TransformMatrix_Inverse[D3DTS_END];
	_float4		m_vCameraPosition;
	_float4		m_vClipPlane[CLIPPLANE_END];

private:
	HRESULT		Update_ReflectView();

public:
	virtual void	Free() override;
};

END