#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint&				Get_MaterialIndex() const { return m_iMaterialIndex; }
	const string&				Get_MeshName() const { return m_strName; }

public:
	virtual HRESULT			Initialize_Prototype(CModel::MODELTYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual HRESULT			Initialize(void* pArg) override;

public:
	void						SetUp_BoneMatrices(_float4x4* pBoneMatrices);

private:
	string						m_strName = "";
	CModel::MODELTYPE		m_eType = CModel::MODELTYPE_END;
	_uint						m_iMaterialIndex = 0;

	_uint						m_iNumMeshBone = 0;
	vector<class CBone*>	m_vecMeshBone;

private:
	HRESULT					Ready_VertexBuffer_NonAnimModel(aiMesh* pAIMesh);
	HRESULT					Ready_VertexBuffer_AnimModel(aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CModel::MODELTYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END