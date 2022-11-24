#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

_matrix CPipeLine::Get_TransformMatrix(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix[eState]);
}

_matrix CPipeLine::Get_TransformMatrix_Inverse(TRANSFORMSTATE eState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(TRANSFORMSTATE eState) const
{
	return m_TransformMatrix[eState];
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
}

void CPipeLine::Tick()
{
	XMStoreFloat4x4(&m_TransformMatrix_Inverse[D3DTS_VIEW], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[D3DTS_VIEW])));
	XMStoreFloat4x4(&m_TransformMatrix_Inverse[D3DTS_PROJ], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[D3DTS_PROJ])));

	memcpy(&m_vCameraPosition, &m_TransformMatrix_Inverse[D3DTS_VIEW]._41, sizeof(_float4));
}

void CPipeLine::Free()
{
}