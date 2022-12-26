#include "stdafx.h"
#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"

CNavigation::CNavigation(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
#ifdef _DEBUG
	, m_pShaderCom(rhs.m_pShaderCom)
#endif // _DEBUG
{
	for (auto& pCell : m_vecCell)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShaderCom);
#endif // _DEBUG

}

HRESULT CNavigation::Initialize_Prototype(const wstring & wstrFilePath)
{
	m_wstrFilePath = wstrFilePath;
	
	_ulong		dwByte = 0;

	HANDLE	hFile = CreateFile(m_wstrFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_float3	vPoint[CCell::POINT_END];

	while (true)
	{
		ReadFile(hFile, vPoint, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (dwByte == 0)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoint, (_int)m_vecCell.size());
		NULL_CHECK_RETURN(pCell, E_FAIL);

		m_vecCell.push_back(pCell);
	}

	CloseHandle(hFile);

	FAILED_CHECK_RETURN(Ready_Neighbor(), E_FAIL);

#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFile/Shader_Navigation.hlsl", CShader::DECLARATION_VTXPOS, VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
#endif // _DEBUG


	return S_OK;
}

HRESULT CNavigation::Initialize(CGameObject * pOwner, void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_tNavigationDesc, pArg, sizeof(NAVIGATIONDESC));

	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);
	
	return S_OK;
}

_bool CNavigation::IsMoveOnNavigation(_fvector vTargetPos)
{
	if (m_tNavigationDesc.iCurrentIndex == -1)
		return false;

	_int		iNeighborIndex = -1;

	if (m_vecCell[m_tNavigationDesc.iCurrentIndex]->IsIn(vTargetPos, iNeighborIndex) == true)
		return true;

	else
	{
		if (iNeighborIndex != -1)
		{
			while (true)
			{
				if (iNeighborIndex == -1)
					return false;

				if (m_vecCell[iNeighborIndex]->IsIn(vTargetPos, iNeighborIndex) == true)
				{
					m_tNavigationDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}
		else
			return false;
	}
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float		fHeight = 0.f;

	if (m_tNavigationDesc.iCurrentIndex != -1)
	{
		fHeight = 0.f;

		m_pShaderCom->Set_RawValue(L"g_fHeight", &fHeight, sizeof(_float));
		m_pShaderCom->Set_RawValue(L"g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}
	else
	{
		fHeight = 0.1f;

		m_pShaderCom->Set_RawValue(L"g_fHeight", &fHeight, sizeof(_float));
		m_pShaderCom->Set_RawValue(L"g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_vecCell[m_tNavigationDesc.iCurrentIndex]->Render(m_pShaderCom);

		return S_OK;
	}

	for (auto& pCell : m_vecCell)
	{
		if (pCell != nullptr)
			pCell->Render(m_pShaderCom);
	}

	return S_OK;
}
#endif //_DEBUG

HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_vecCell)
	{
		for (auto& pDestCell : m_vecCell)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Point(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_AB, pDestCell);

			else if (true == pDestCell->Compare_Point(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_BC, pDestCell);

			else if (true == pDestCell->Compare_Point(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrFilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrFilePath)))
	{
		MSG_BOX("Failed to Create : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(CGameObject * pOwner, void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_vecCell)
		Safe_Release(pCell);
	m_vecCell.clear();

#ifdef _DEBUG
	Safe_Release(m_pShaderCom);
#endif // _DEBUG

}
