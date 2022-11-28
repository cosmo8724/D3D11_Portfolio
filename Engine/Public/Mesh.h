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
	virtual HRESULT			Initialize_Prototype(CModel::MODELTYPE eType, aiMesh* pAIMesh);
	virtual HRESULT			Initialize(void* pArg) override;

private:
	CModel::MODELTYPE		m_eType = CModel::MODELTYPE_END;
	_uint						m_iMaterialIndex = 0;
	string						m_strName = "";
	_uint						m_iNumBones = 0;

private:
	HRESULT					Ready_VertexBuffer_NonAnimModel(aiMesh* pAIMesh);
	HRESULT					Ready_VertexBuffer_AnimModel(aiMesh* pAIMesh);

public:
	static CMesh*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CModel::MODELTYPE eType, aiMesh* pAIMesh);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END