#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	_float3*						Get_TerrainPosition() { return m_pPos; }
	_uint							Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint							Get_NumVerticesZ() { return m_iNumVerticesZ; }

public:
	HRESULT						Initialize_Prototype(const wstring wstrHeightMapFilePath);
	virtual HRESULT				Initialize(class CGameObject* pOwner, void* pArg) override;
	void							Tick(_double dTimeDelta);

public:
	void							Culling(_fmatrix matWorld);

private:
	_uint							m_iNumVerticesX = 0;
	_uint							m_iNumVerticesZ = 0;

	_float3*						m_pPos = nullptr;
	FACEINDICES32*				m_pIndices = nullptr;

	class CQuadTree*			m_pQuadTree = nullptr;

public:
	static	CVIBuffer_Terrain*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring wstrHeightMapFilePath);
	virtual CComponent*		Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void					Free() override;
};

END