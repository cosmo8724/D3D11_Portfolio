#include "stdafx.h"
#include "..\Public\Cell.h"
#include "GameInstance.h"

CCell::CCell(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_vector CCell::Get_CellHeight(_float4 vTargetPos)
{	
	_float4	vRayPos, vRayDir;
	vRayPos = vTargetPos;
	vRayPos.y += 50.f;

	vRayDir = vTargetPos - vRayPos;
	vRayDir.Normalize();

	_float		fDist = 0.f;

	_vector	vPointA = XMVectorSetW(m_vPoint[POINT_A], 1.f);
	_vector	vPointB = XMVectorSetW(m_vPoint[POINT_B], 1.f);
	_vector	vPointC = XMVectorSetW(m_vPoint[POINT_C], 1.f);

	if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDist))
		return vRayPos + vRayDir * fDist;

	return XMLoadFloat4(&vTargetPos);
}

void CCell::Get_BlockedLine(NEIGHBOR eNeighbor, _float4 & vBlockedLine, _float4 & vBlockedLineNormal)
{
	_vector		vLine = XMLoadFloat3(&m_vPoint[(eNeighbor + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoint[eNeighbor]);
	_vector		vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));

	XMStoreFloat4(&vBlockedLine, vLine);
	XMStoreFloat4(&vBlockedLineNormal, vNormal);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	memcpy(m_vPoint, pPoints, sizeof(_float3) * POINT_END);
	m_iIndex = iIndex;
	 
#ifdef _DEBUG
	m_pVIBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoint);
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	m_pVIBufferCircleCom[POINT_A] = CVIBuffer_Cell_Circle::Create(m_pDevice, m_pContext, m_vPoint[POINT_A]);
	m_pVIBufferCircleCom[POINT_B] = CVIBuffer_Cell_Circle::Create(m_pDevice, m_pContext, m_vPoint[POINT_B]);
	m_pVIBufferCircleCom[POINT_C] = CVIBuffer_Cell_Circle::Create(m_pDevice, m_pContext, m_vPoint[POINT_C]);
#endif // _DEBUG


	return S_OK;
}

void CCell::ImGui_RenderProperty()
{
	ImGui::InputFloat3("POINT A", &m_vPoint[POINT_A].x);
	ImGui::InputFloat3("POINT B", &m_vPoint[POINT_B].x);
	ImGui::InputFloat3("POINT C", &m_vPoint[POINT_C].x);
	ImGui::NewLine();
	ImGui::InputInt3("Neighbor", m_iNeighborIndex, ImGuiInputTextFlags_ReadOnly);

	char*		pStateName[STATE_END + 1] = { "State_Ocean", "State_Ground", "State_Wall", "None" };

	ImGui::Combo("State", (_int*)&m_eState, pStateName, STATE_END + 1);
}

_bool CCell::Compare_Point(const _float3 & SourPoint, const _float3 & DestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_C]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_A]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoint[POINT_B]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	return false;
}

_bool CCell::Compare_Height(_fvector vTargetPos)
{
	_float		fAvgHeight = 0.f;

	for (_uint i = 0; i < POINT_END; ++i)
		fAvgHeight += m_vPoint[i].y;

	fAvgHeight /= 3.f;

	if (fAvgHeight > XMVectorGetY(vTargetPos))
		return true;
	else
		return false;
}

_bool CCell::IsIn(_fvector vTargetPos, _int & iNeighborIndex, _float4 & vBlockedLine, _float4 & vBlockedLineNormal)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoint[(i + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoint[i]);
		_vector		vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));
		_vector		vDir = XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vPoint[i]));

		if (0.f < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			iNeighborIndex = m_iNeighborIndex[i];

			if (iNeighborIndex == -1)
			{
				XMStoreFloat4(&vBlockedLine, vLine);
				XMStoreFloat4(&vBlockedLineNormal, vNormal);
			}

			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(CShader * pShader)
{
	NULL_CHECK_RETURN(m_pVIBufferCom, E_FAIL);

	_float4x4		matWorld;
	XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
	
	pShader->Set_Matrix(L"g_matWorld", &matWorld);
	pShader->Set_Matrix(L"g_matView", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	pShader->Set_Matrix(L"g_matProj", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	pShader->Begin(0);

	m_pVIBufferCom->Render();

	for (_uint i = 0; i < (_uint)POINT_END; ++i)
		m_pVIBufferCircleCom[i]->Render();

	return S_OK;
}
#endif

CCell * CCell::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Create : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);

	for (_uint i = 0; i < (_uint)POINT_END; ++i)
		Safe_Release(m_pVIBufferCircleCom[i]);
#endif // _DEBUG
}
