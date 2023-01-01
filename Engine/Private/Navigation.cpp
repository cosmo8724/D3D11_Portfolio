#include "stdafx.h"
#include "..\Public\Navigation.h"
#include "GameInstance.h"
#include "Cell.h"
#include "Shader.h"
#include "Json/json.hpp"
#include <fstream>

CNavigation::CNavigation(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{ 
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_vecCell(rhs.m_vecCell)
	, m_tNavigationDesc(rhs.m_tNavigationDesc)
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
	if (wstrFilePath != L"")
	{
		m_wstrFilePath = wstrFilePath;

		Json	jCells;

		string	strFilePath = "";
		strFilePath.assign(m_wstrFilePath.begin(), m_wstrFilePath.end());

		ifstream	file(strFilePath.c_str());
		file >> jCells;
		file.close();

		_uint	iCellCount;
		jCells["Cell Count"].get_to<_uint>(iCellCount);

		for (auto jCell : jCells["Cells"])
		{
			_float3	vPoint[CCell::POINT_END];

			jCell["Point A"][0].get_to<_float>(vPoint[CCell::POINT_A].x);
			jCell["Point A"][1].get_to<_float>(vPoint[CCell::POINT_A].y);
			jCell["Point A"][2].get_to<_float>(vPoint[CCell::POINT_A].z);
			jCell["Point B"][0].get_to<_float>(vPoint[CCell::POINT_B].x);
			jCell["Point B"][1].get_to<_float>(vPoint[CCell::POINT_B].y);
			jCell["Point B"][2].get_to<_float>(vPoint[CCell::POINT_B].z);
			jCell["Point C"][0].get_to<_float>(vPoint[CCell::POINT_C].x);
			jCell["Point C"][1].get_to<_float>(vPoint[CCell::POINT_C].y);
			jCell["Point C"][2].get_to<_float>(vPoint[CCell::POINT_C].z);

			CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoint, (_int)m_vecCell.size());
			NULL_CHECK_RETURN(pCell, E_FAIL);

			m_vecCell.push_back(pCell);
		}
	}
	
	FAILED_CHECK_RETURN(Ready_Neighbor(), E_FAIL);
	
#ifdef _DEBUG
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../Bin/Shader/Shader_Navigation.hlsl", CShader::DECLARATION_VTXPOS, VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
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

HRESULT CNavigation::Add_Cell(_float3 * vPoints)
{
	CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_vecCell.size());
	NULL_CHECK_RETURN(pCell, E_FAIL);

	m_vecCell.push_back(pCell);
	FAILED_CHECK_RETURN(Ready_Neighbor(), E_FAIL);

	return S_OK;
}

HRESULT CNavigation::Delete_Cell(_int iIndex)
{
	if (iIndex < 0 || iIndex >= (_int)m_vecCell.size())
		return E_FAIL;

	auto	iter = m_vecCell.begin();

	for (_int i = 0; i < iIndex; ++i)
		++iter;

	Safe_Release(*iter);
	m_vecCell.erase(iter);

	return S_OK;
}

HRESULT CNavigation::Save_Cell(string strSaveFilePath)
{
	Json	jCells;
	Json	jCell;

	jCells["Cell Count"] = (_uint)m_vecCell.size();

	for (auto& pCell : m_vecCell)
	{
		jCell["Index"] = pCell->Get_Index();
		jCell["Point A"].push_back(pCell->Get_Point(CCell::POINT_A).x);
		jCell["Point A"].push_back(pCell->Get_Point(CCell::POINT_A).y);
		jCell["Point A"].push_back(pCell->Get_Point(CCell::POINT_A).z);
		jCell["Point B"].push_back(pCell->Get_Point(CCell::POINT_B).x);
		jCell["Point B"].push_back(pCell->Get_Point(CCell::POINT_B).y);
		jCell["Point B"].push_back(pCell->Get_Point(CCell::POINT_B).z);
		jCell["Point C"].push_back(pCell->Get_Point(CCell::POINT_C).x);
		jCell["Point C"].push_back(pCell->Get_Point(CCell::POINT_C).y);
		jCell["Point C"].push_back(pCell->Get_Point(CCell::POINT_C).z);
		jCell["Neighbor Index"].push_back(pCell->Get_NeighbotIndex()[CCell::NEIGHBOR_AB]);
		jCell["Neighbor Index"].push_back(pCell->Get_NeighbotIndex()[CCell::NEIGHBOR_BC]);
		jCell["Neighbor Index"].push_back(pCell->Get_NeighbotIndex()[CCell::NEIGHBOR_CA]);

		jCells["Cells"].push_back(jCell);
		jCell.clear();
	}

	ofstream	file(strSaveFilePath.c_str());
	file << jCells;
	file.close();

	jCells.clear();

	return S_OK;
}

HRESULT CNavigation::Find_NearBy_Point(_float3 & vPoint)
{
	_float3	vPoints[CCell::POINT_END];

	for (auto& pCell : m_vecCell)
	{
		ZeroMemory(&vPoints, sizeof(_float3) * CCell::POINT_END);

		vPoints[CCell::POINT_A] = pCell->Get_Point(CCell::POINT_A);
		vPoints[CCell::POINT_B] = pCell->Get_Point(CCell::POINT_B);
		vPoints[CCell::POINT_C] = pCell->Get_Point(CCell::POINT_C);
		_vector	vDir;
		_float		fDist = 10000.f;
		
		vDir = vPoints[CCell::POINT_A] - vPoint;
		fDist = XMVectorGetX(XMVector3Length(vDir));
		if (fDist < 0.15f)
		{
			vPoint = vPoints[CCell::POINT_A];
			break;
		}

		vDir = vPoints[CCell::POINT_B] - vPoint;
		fDist = XMVectorGetX(XMVector3Length(vDir));
		if (fDist < 0.15f)
		{
			vPoint = vPoints[CCell::POINT_B];
			break;
		}

		vDir = vPoints[CCell::POINT_C] - vPoint;
		fDist = XMVectorGetX(XMVector3Length(vDir));
		if (fDist < 0.15f)
		{
			vPoint = vPoints[CCell::POINT_C];
			break;
		}
	}

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

	if (m_tNavigationDesc.iCurrentIndex == -1)
	{
		fHeight = 0.1f;

		m_pShaderCom->Set_RawValue(L"g_fHeight", &fHeight, sizeof(_float));
		m_pShaderCom->Set_RawValue(L"g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}
	else
	{
		fHeight = 0.15f;

		m_pShaderCom->Set_RawValue(L"g_fHeight", &fHeight, sizeof(_float));
		m_pShaderCom->Set_RawValue(L"g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_vecCell[m_tNavigationDesc.iCurrentIndex]->Render(m_pShaderCom);

		return S_OK;
	}

	if (m_vecCell.empty())
	{
		_float4x4		matWorld;
		XMStoreFloat4x4(&matWorld, XMMatrixIdentity());

		m_pShaderCom->Set_Matrix(L"g_matWorld", &matWorld);
		m_pShaderCom->Set_Matrix(L"g_matView", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
		m_pShaderCom->Set_Matrix(L"g_matProj", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

		m_pShaderCom->Begin(0);
	}
	else
	{
		for (auto& pCell : m_vecCell)
		{
			if (pCell != nullptr)
				pCell->Render(m_pShaderCom);
		}
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
