#include "stdafx.h"
#include "..\Public\QuadTree.h"
#include "Frustum.h"
#include "PipeLine.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLeftTop, _uint iRightTop, _uint iRightBottom, _uint iLeftBottom)
{
	m_iCorner[CORNER_LEFTTOP] = iLeftTop;
	m_iCorner[CORNER_RIGHTTOP] = iRightTop;
	m_iCorner[CORNER_RIGHTBOTTOM] = iRightBottom;
	m_iCorner[CORNER_LEFTBOTTOM] = iLeftBottom;

	if (m_iCorner[CORNER_RIGHTBOTTOM] - m_iCorner[CORNER_LEFTBOTTOM] == 1)
		return S_OK;

	m_iCenterIndex = (m_iCorner[CORNER_LEFTTOP] + m_iCorner[CORNER_RIGHTBOTTOM]) >> 1;

	_uint	iLeftCenter, iTopCenter, iRightCenter, iBottomCenter;

	iLeftCenter = (m_iCorner[CORNER_LEFTTOP] + m_iCorner[CORNER_LEFTBOTTOM]) >> 1;
	iTopCenter = (m_iCorner[CORNER_LEFTTOP] + m_iCorner[CORNER_RIGHTTOP]) >> 1;
	iRightCenter = (m_iCorner[CORNER_RIGHTTOP] + m_iCorner[CORNER_RIGHTBOTTOM]) >> 1;
	iBottomCenter = (m_iCorner[CORNER_LEFTBOTTOM] + m_iCorner[CORNER_RIGHTBOTTOM]) >> 1;

	m_pChild[CHILD_LEFTTOP] = CQuadTree::Create(m_iCorner[CORNER_LEFTTOP], iTopCenter, m_iCenterIndex, iLeftCenter);
	m_pChild[CHILD_RIGHTTOP] = CQuadTree::Create(iTopCenter, m_iCorner[CORNER_RIGHTTOP], iRightCenter, m_iCenterIndex);
	m_pChild[CHILD_RIGHTBOTTOM] = CQuadTree::Create(m_iCenterIndex, iRightCenter, m_iCorner[CORNER_RIGHTBOTTOM], iBottomCenter);
	m_pChild[CHILD_LEFTBOTTOM] = CQuadTree::Create(iLeftCenter, m_iCenterIndex, iBottomCenter, m_iCorner[CORNER_LEFTBOTTOM]);

	return S_OK;
}

void CQuadTree::Culling(CFrustum * pFrustum, const _float3 * pVtxPos, FACEINDICES32 * pIndices, _uint & iNumFaces)
{
	//_bool		IsIn[CORNER_END] = {
	//	pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_LEFTTOP]]), 0.f),
	//	pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_RIGHTTOP]]), 0.f),
	//	pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_RIGHTBOTTOM]]), 0.f),
	//	pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_LEFTBOTTOM]]), 0.f)
	//};

	///* 점 4개가 모두 절두체 안에 들어와 있을 때, 포함하고 있는 인덱스 모두 추가 */
	//if (IsIn[CORNER_LEFTTOP] && IsIn[CORNER_RIGHTTOP] && IsIn[CORNER_RIGHTBOTTOM] && IsIn[CORNER_LEFTBOTTOM])
	//{
	//	_uint	iNumVerticesX = m_iCorner[CORNER_RIGHTBOTTOM] - m_iCorner[CORNER_LEFTBOTTOM] + 1;
	//	_uint	iNumVerticesZ = iNumVerticesX;//(m_iCorner[CORNER_LEFTTOP] - m_iCorner[CORNER_LEFTBOTTOM]) / iNumVerticesX + 1;

	//	for (_uint z = 0; z < iNumVerticesZ - 1; ++z)
	//	{
	//		for (_uint x = 0; x < iNumVerticesX - 1; ++x)
	//		{
	//			_uint	iIndex = z * iNumVerticesX + x + m_iCorner[CORNER_LEFTBOTTOM];

	//			_uint			iIndices[4] = {
	//				iIndex + iNumVerticesX,
	//				iIndex + iNumVerticesX + 1,
	//				iIndex + 1,
	//				iIndex
	//			};

	//			pIndices[iNumFaces]._0 = iIndices[0];
	//			pIndices[iNumFaces]._1 = iIndices[1];
	//			pIndices[iNumFaces]._2 = iIndices[2];
	//			++iNumFaces;

	//			pIndices[iNumFaces]._0 = iIndices[0];
	//			pIndices[iNumFaces]._1 = iIndices[2];
	//			pIndices[iNumFaces]._2 = iIndices[3];
	//			++iNumFaces;
	//		}
	//	}

	//	return;
	//}
	//else
	{
		if (m_pChild[CHILD_LEFTTOP] == nullptr || LevelOfDetail(pVtxPos) == true)
		{
			_bool		IsIn[CORNER_END] = {
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_LEFTTOP]]), 2.f),
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_RIGHTTOP]]), 2.f),
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_RIGHTBOTTOM]]), 2.f),
				pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCorner[CORNER_LEFTBOTTOM]]), 2.f)
			};

			_bool		IsDraw[NEIGHBOR_END] = { true, true, true, true };

			for (_uint i = 0; i < NEIGHBOR_END; ++i)
			{
				if (m_pNeighbor[i] != nullptr)
					IsDraw[i] = m_pNeighbor[i]->LevelOfDetail(pVtxPos);
			}

			_uint	iLeftCenter, iTopCenter, iRightCenter, iBottomCenter;

			iLeftCenter = (m_iCorner[CORNER_LEFTTOP] + m_iCorner[CORNER_LEFTBOTTOM]) >> 1;
			iTopCenter = (m_iCorner[CORNER_LEFTTOP] + m_iCorner[CORNER_RIGHTTOP]) >> 1;
			iRightCenter = (m_iCorner[CORNER_RIGHTTOP] + m_iCorner[CORNER_RIGHTBOTTOM]) >> 1;
			iBottomCenter = (m_iCorner[CORNER_LEFTBOTTOM] + m_iCorner[CORNER_RIGHTBOTTOM]) >> 1;

			if (IsDraw[NEIGHBOR_LEFT] && IsDraw[NEIGHBOR_TOP] && IsDraw[NEIGHBOR_RIGHT] && IsDraw[NEIGHBOR_BOTTOM])
			{
				if (IsIn[CORNER_LEFTTOP] == true || IsIn[CORNER_RIGHTTOP] == true || IsIn[CORNER_RIGHTBOTTOM] == true)
				{
					pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
					pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTTOP];
					pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
					++iNumFaces;
				}
				if (IsIn[CORNER_LEFTTOP] == true || IsIn[CORNER_RIGHTBOTTOM] == true || IsIn[CORNER_LEFTBOTTOM] == true)
				{
					pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
					pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTBOTTOM];
					pIndices[iNumFaces]._2 = m_iCorner[CORNER_LEFTBOTTOM];
					++iNumFaces;
				}

				return;
			}
			else if ((IsDraw[NEIGHBOR_LEFT] == false || IsDraw[NEIGHBOR_BOTTOM] == false) && IsDraw[NEIGHBOR_TOP] == true && IsDraw[NEIGHBOR_RIGHT] == true)
			{
				if (IsIn[CORNER_LEFTTOP] == true || IsIn[CORNER_RIGHTTOP] == true || IsIn[CORNER_RIGHTBOTTOM] == true)
				{
					pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
					pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTTOP];
					pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
					++iNumFaces;
				}
				if (IsIn[CORNER_LEFTTOP] || IsIn[CORNER_RIGHTBOTTOM] || IsIn[CORNER_LEFTBOTTOM])
				{
					if (IsDraw[NEIGHBOR_LEFT] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = iLeftCenter;
						++iNumFaces;

						pIndices[iNumFaces]._0 = iLeftCenter;
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_LEFTBOTTOM];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_LEFTBOTTOM];
						++iNumFaces;
					}

					if (IsDraw[NEIGHBOR_BOTTOM] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTBOTTOM];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = iBottomCenter;
						++iNumFaces;

						pIndices[iNumFaces]._0 = iBottomCenter;
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTBOTTOM];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
						++iNumFaces;
					}
				}

				return;
			}
			else if (IsDraw[NEIGHBOR_LEFT] == true && IsDraw[NEIGHBOR_BOTTOM] == true && (IsDraw[NEIGHBOR_TOP] == false || IsDraw[NEIGHBOR_RIGHT] == false))
			{
				if (IsIn[CORNER_LEFTTOP] || IsIn[CORNER_RIGHTTOP] || IsIn[CORNER_RIGHTBOTTOM])
				{
					if (IsDraw[NEIGHBOR_TOP] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = iTopCenter;
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;

						pIndices[iNumFaces]._0 = m_iCenterIndex;
						pIndices[iNumFaces]._1 = iTopCenter;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTTOP];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTTOP];
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;
					}

					if (IsDraw[NEIGHBOR_RIGHT] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_RIGHTTOP];
						pIndices[iNumFaces]._1 = iRightCenter;
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;

						pIndices[iNumFaces]._0 = m_iCenterIndex;
						pIndices[iNumFaces]._1 = iRightCenter;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_RIGHTTOP];
						pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTBOTTOM];
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;
					}
				}
				if (IsIn[CORNER_LEFTTOP] == true || IsIn[CORNER_RIGHTBOTTOM] == true || IsIn[CORNER_LEFTBOTTOM] == true)
				{
					pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
					pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTBOTTOM];
					pIndices[iNumFaces]._2 = m_iCorner[CORNER_LEFTBOTTOM];
					++iNumFaces;
				}

				return;
			}
			else
			{
				if (IsIn[CORNER_LEFTTOP] || IsIn[CORNER_RIGHTBOTTOM] || IsIn[CORNER_LEFTBOTTOM])
				{
					if (IsDraw[NEIGHBOR_LEFT] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = iLeftCenter;
						++iNumFaces;

						pIndices[iNumFaces]._0 = iLeftCenter;
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_LEFTBOTTOM];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_LEFTBOTTOM];
						++iNumFaces;
					}

					if (IsDraw[NEIGHBOR_BOTTOM] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTBOTTOM];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = iBottomCenter;
						++iNumFaces;

						pIndices[iNumFaces]._0 = iBottomCenter;
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTBOTTOM];
						pIndices[iNumFaces]._1 = m_iCenterIndex;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
						++iNumFaces;
					}
				}

				if (IsIn[CORNER_LEFTTOP] || IsIn[CORNER_RIGHTTOP] || IsIn[CORNER_RIGHTBOTTOM])
				{
					if (IsDraw[NEIGHBOR_TOP] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = iTopCenter;
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;

						pIndices[iNumFaces]._0 = m_iCenterIndex;
						pIndices[iNumFaces]._1 = iTopCenter;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTTOP];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_LEFTTOP];
						pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTTOP];
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;
					}

					if (IsDraw[NEIGHBOR_RIGHT] == false)
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_RIGHTTOP];
						pIndices[iNumFaces]._1 = iRightCenter;
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;

						pIndices[iNumFaces]._0 = m_iCenterIndex;
						pIndices[iNumFaces]._1 = iRightCenter;
						pIndices[iNumFaces]._2 = m_iCorner[CORNER_RIGHTBOTTOM];
						++iNumFaces;
					}
					else
					{
						pIndices[iNumFaces]._0 = m_iCorner[CORNER_RIGHTTOP];
						pIndices[iNumFaces]._1 = m_iCorner[CORNER_RIGHTBOTTOM];
						pIndices[iNumFaces]._2 = m_iCenterIndex;
						++iNumFaces;
					}
				}

				return;
			}
		}

		_float		fRange = XMVectorGetX(XMVector3Length(XMLoadFloat3(&(pVtxPos[m_iCorner[CORNER_LEFTTOP]] - pVtxPos[m_iCenterIndex]))));

		if (pFrustum->IsInFrustum_Local(XMLoadFloat3(&pVtxPos[m_iCenterIndex]), fRange))
		{
			for (_uint i = 0; i < CHILD_END; ++i)
			{
				if (m_pChild[i] != nullptr)
					m_pChild[i]->Culling(pFrustum, pVtxPos, pIndices, iNumFaces);
			}
		}
	}

	return;
}

void CQuadTree::Make_Neighbor()
{
	if (m_pChild[CHILD_LEFTTOP]->m_pChild[CHILD_LEFTTOP] == nullptr)
		return;

	m_pChild[CHILD_LEFTTOP]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RIGHTTOP];
	m_pChild[CHILD_LEFTTOP]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LEFTBOTTOM];

	m_pChild[CHILD_RIGHTTOP]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LEFTTOP];
	m_pChild[CHILD_RIGHTTOP]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RIGHTBOTTOM];

	m_pChild[CHILD_RIGHTBOTTOM]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LEFTBOTTOM];
	m_pChild[CHILD_RIGHTBOTTOM]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RIGHTTOP];

	m_pChild[CHILD_LEFTBOTTOM]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RIGHTBOTTOM];
	m_pChild[CHILD_LEFTBOTTOM]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LEFTTOP];

	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LEFTTOP]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RIGHTTOP];
		m_pChild[CHILD_LEFTBOTTOM]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RIGHTBOTTOM];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LEFTTOP]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LEFTBOTTOM];
		m_pChild[CHILD_RIGHTTOP]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RIGHTBOTTOM];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RIGHTTOP]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LEFTTOP];
		m_pChild[CHILD_RIGHTBOTTOM]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LEFTBOTTOM];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LEFTBOTTOM]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LEFTTOP];
		m_pChild[CHILD_RIGHTBOTTOM]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RIGHTTOP];
	}

	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_Neighbor();
	}
}

_bool CQuadTree::LevelOfDetail(const _float3 * pVtxPos)
{
	CPipeLine*	pPipeLine = CPipeLine::GetInstance();

	_vector		vCameraPos = XMLoadFloat4(&pPipeLine->Get_CameraPosition());
	_float			fDistFromCamera = XMVectorGetX(XMVector3Length(vCameraPos - XMLoadFloat3(&pVtxPos[m_iCenterIndex])));
	_float			fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&(pVtxPos[m_iCorner[CORNER_RIGHTBOTTOM]] - pVtxPos[m_iCorner[CORNER_LEFTBOTTOM]]))));

	if (fWidth < fDistFromCamera * 0.2f)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iLeftTop, _uint iRightTop, _uint iRightBottom, _uint iLeftBottom)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLeftTop, iRightTop, iRightBottom, iLeftBottom)))
	{
		MSG_BOX("Failed to Create : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pChild : m_pChild)
		Safe_Release(pChild);
}