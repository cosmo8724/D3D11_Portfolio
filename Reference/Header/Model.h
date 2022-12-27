#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { MODEL_NONANIM, MODEL_ANIM, MODELTYPE_END };
	enum LERPTYPE { LERP_BEGIN, LERP_CONTINUE, LERPTYPE_END };

private:
	CModel(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	HRESULT							Save_Model(const char* pSaveFileDirectory);
	const MODELTYPE&				Get_ModelType() const { return m_eType; }
	const _uint&						Get_NumMeshes() const { return m_iNumMeshes; }
	class CMesh*						Get_Mesh(const string& strMeshName);
	_uint								Get_NumAnimations() const { return m_iNumAnimations; }
	_matrix							Get_PivotMatrix() const { return XMLoadFloat4x4(&m_matPivot); }
	class CBone*						Get_BoneFromEntireBone(const string & strBoneName);
	_matrix							Get_BoneMatrix(const string& strBoneName);
	_matrix							Get_OffsetMatrix(const string & strBoneName);
	_uint								Get_LastAnimationIndex() { return m_iLastAnimationIndex; }
	_bool								Get_AnimationFinish();
	_float								Get_AnimationProgress();
	void								Set_CurAnimationIndex(_uint iAnimationIndex);
	void								Set_LerpTime(_float fLerpTime) { m_fAnimChangeTime = fLerpTime; }
	void								Reset_Animation();
	HRESULT							Check_MeshSize(const string& strMeshName, _float& Xmin, _float& Xmax, _float& Ymin, _float& Ymax, _float& Zmin, _float& Zmax);

public:
	virtual HRESULT					Initialize_Prototype(MODELTYPE eType, const char* pModelFilePath, _fmatrix matPivot);
	virtual HRESULT					Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual void						ImGui_RenderProperty() override;
	void								ImGui_RenderAnimation();

public:
	void								Play_Animation(_double dTimeDelta, LERPTYPE eType = LERP_BEGIN);
	HRESULT							Bind_Material(class CShader* pShaderCom, _uint iMeshIndex, aiTextureType eType, const wstring& wstrConstantName);
	HRESULT							Render(class CShader* pShaderCom, _uint iMeshIndex, const wstring& wstrBoneConstantName = L"", _uint iPassIndex = 0);

private:
	const aiScene*					m_pAIScene = nullptr;
	Importer							m_Importer;
	MODELTYPE						m_eType = MODELTYPE_END;
	_float4x4							m_matPivot;

	_uint								m_iNumMeshes = 0;
	vector<class CMesh*>			m_vecMesh;
	typedef vector<class CMesh*>			MESHES;

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>		m_vecMaterial;
	typedef vector<MODELMATERIAL>		MATERIALS;

	_uint								m_iNumEntireBone = 0;
	vector<class CBone*>			m_vecEntireBone;
	typedef vector<class CBone*>			BONES;

	_bool								m_bAnimFinished = false;
	_uint								m_iLastAnimationIndex = 0;
	_uint								m_iCurAnimationIndex = 0;
	_float								m_fAnimChangeTime = 0.2f;
	_float								m_fCurAnimChangeTime = 0.2f;

	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>		m_vecAnimation;
	typedef vector<class CAnimation*>	ANIMATIONS;

	DWORD							m_dwBeginBoneData = 0;

private:
	HRESULT							Ready_Bones(aiNode* pAINode, class CBone* pParent);
	HRESULT							Ready_Bones(HANDLE& hFile, DWORD& dwByte, class CBone* pParent);
	HRESULT							Ready_MeshContainers();
	HRESULT							Ready_Materials(const char* pModelFIlePath);
	HRESULT							Ready_Animations();

	HRESULT							Load_MeshMaterial(const wstring& wstrModelFilePath);
	HRESULT							Load_BoneAnimation(HANDLE& hFile, DWORD& dwByte);

public:
	static CModel*					Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MODELTYPE eType, const char* pModelFilePath, _fmatrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.0f)));
	virtual CComponent*			Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void						Free() override;
};

END