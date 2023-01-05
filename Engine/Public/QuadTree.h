#pragma once
#include "Base.h"

BEGIN(Engine)

class CQuadTree final : public CBase
{
public:
	enum CHILD { CHILD_LEFTTOP, CHILD_RIGHTTOP, CHILD_RIGHTBOTTOM, CHILD_LEFTBOTTOM, CHILD_END };
	enum CORNER { CORNER_LEFTTOP, CORNER_RIGHTTOP, CORNER_RIGHTBOTTOM, CORNER_LEFTBOTTOM, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };

private:
	CQuadTree();
	virtual ~CQuadTree() = default;

public:
	HRESULT				Initialize(_uint iLeftTop, _uint iRightTop, _uint iRightBottom, _uint iLeftBottom);
	void					Culling(class CFrustum* pFrustum, const _float3* pVtxPos, FACEINDICES32* pIndices, _uint& iNumFaces);
	void					Make_Neighbor();

private:
	CQuadTree*			m_pChild[CHILD_END] = { nullptr, nullptr, nullptr, nullptr };
	CQuadTree*			m_pNeighbor[NEIGHBOR_END] = { nullptr, nullptr, nullptr, nullptr };
	_uint					m_iCorner[CORNER_END] = { 0, 0, 0, 0 };

	_uint					m_iCenterIndex = 0;
	_float					m_fRange = 0.f;

private:
	_bool					LevelOfDetail(const _float3* pVtxPos);

public:
	static CQuadTree*	Create(_uint iLeftTop, _uint iRightTop, _uint iRightBottom, _uint iLeftBottom);
	virtual void			Free() override;
};

END