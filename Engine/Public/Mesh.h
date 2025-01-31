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
	HRESULT					Save_Mesh(HANDLE& hFile, DWORD& dwByte);
	HRESULT					Save_MeshBones(HANDLE& hFile, DWORD& dwByte);
	HRESULT					Load_Mesh(HANDLE& hFile, DWORD& dwByte);
	const _uint&				Get_MaterialIndex() const { return m_iMaterialIndex; }
	const string&				Get_MeshName() const { return m_strName; }
	void						Check_MeshSize(_float& Xmin, _float& Xmax, _float& Ymin, _float& Ymax, _float& Zmin, _float& Zmax);

public:
	virtual HRESULT			Initialize_Prototype(CModel::MODELTYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	void						SetUp_MeshBones(class CModel* pModel);
	HRESULT					SetUp_MeshBones(HANDLE& hFile, DWORD& dwByte, class CModel* pModel);
	void						SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix matPivot);
	pair<_bool, _float>		Picking(HWND& hWnd, class CTransform* pTransformCom, _float3& vPickingPoint);
	_bool						IsInFrustum(_fmatrix matPivot, _fmatrix matWorld);

private:
	aiMesh*					m_pAIMesh = nullptr;
	string						m_strName = "";
	CModel::MODELTYPE		m_eType = CModel::MODELTYPE_END;
	_uint						m_iMaterialIndex = 0;

	_uint						m_iNumMeshBone = 0;
	vector<class CBone*>	m_vecMeshBone;

	VTXMODEL*				m_pNonAnimVertices = nullptr;
	VTXANIMMODEL*			m_pAnimVertices = nullptr;
	FACEINDICES32*			m_pIndices = nullptr;

private:
	HRESULT					Ready_VertexBuffer_NonAnimModel(aiMesh* pAIMesh, class CModel* pModel);
	HRESULT					Ready_VertexBuffer_AnimModel(aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMesh*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, CModel::MODELTYPE eType, aiMesh* pAIMesh, class CModel* pModel);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END