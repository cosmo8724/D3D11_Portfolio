#include "stdafx.h"
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

_float4x4 CPipeLine::Get_TransformFloat4x4_Inverse(TRANSFORMSTATE eState) const
{
	return m_TransformMatrix_Inverse[eState];
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
}

void CPipeLine::Tick()
{
	XMStoreFloat4x4(&m_TransformMatrix_Inverse[D3DTS_VIEW], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[D3DTS_VIEW])));
	XMStoreFloat4x4(&m_TransformMatrix_Inverse[D3DTS_PROJ], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[D3DTS_PROJ])));

	Update_ReflectView();

	memcpy(&m_vCameraPosition, &m_TransformMatrix_Inverse[D3DTS_VIEW]._41, sizeof(_float4));
}

HRESULT CPipeLine::Update_ReflectView()
{
	/*_vector	vCamLook, vTargetPos;
	_vector	vCamRight;

	vCamLook = XMLoadFloat4x4(&dynamic_cast<CDynamic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->front())->Get_WorldMatrix()).r[2];
	vCamRight = XMLoadFloat4x4(&dynamic_cast<CDynamic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->front())->Get_WorldMatrix()).r[0];

	vTargetPos = XMLoadFloat4(&m_vCameraPosition) + XMVector3Normalize(vCamLook);

	_float		fReflectionCamYCoord = -XMVectorGetY(m_vCameraPosition) + 2.f * m_fWaterLevel;
	_vector	vReflectionCamPos = XMVectorSet(XMVectorGetX(m_vCameraPosition), fReflectionCamYCoord, XMVectorGetZ(vCamPos), 1.f);

	float		fReflectionTargetYCoord = -XMVectorGetY(vTargetPos) + 2 * m_fWaterLevel;
	_vector	vReflectionCamTarget = XMVectorSet(XMVectorGetX(vTargetPos), fReflectionTargetYCoord, XMVectorGetZ(vTargetPos), 1.f);

	_vector	vForward = vReflectionCamTarget - vReflectionCamPos;
	_vector	vReflectionCamUp = XMVector3Cross(vCamRight, vForward);

	m_matReflectionView = XMMatrixLookAtLH(vReflectionCamPos, vReflectionCamTarget, vReflectionCamUp);*/

	return S_OK;
}

void CPipeLine::Free()
{
}