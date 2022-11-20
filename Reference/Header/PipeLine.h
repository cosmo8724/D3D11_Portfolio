#pragma once
#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_matrix		Get_TransformMatrix(TRANSFORMSTATE eState) const;
	_matrix		Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const;
	_float4x4		Get_TransformFloat4x4(TRANSFORMSTATE eState) const;

public:
	void			Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix);

public:
	void			Tick();

private:
	_float4x4		m_TransformMatrix[D3DTS_END];
	_float4x4		m_TransformMatrix_Inverse[D3DTS_END];
	_float4		m_vCameraPosition;

public:
	virtual void	Free() override;
};

END