#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { MODEL_NONANIM, MODEL_ANIM, MODELTYPE_END };

private:
	CModel(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	const _uint&						Get_NumMeshes() const { return m_iNumMeshes; }

public:
	virtual HRESULT					Initialize_Prototype(MODELTYPE eType, const char* pModelFilePath);
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void						ImGui_RenderProperty() override;

public:
	HRESULT							Bind_Material(class CShader* pShaderCom, _uint iMeshIndex, aiTextureType eType, const wstring& wstrConstantName);
	HRESULT							Render(class CShader* pShaderCom, _uint iMeshIndex);

public:
	const aiScene*					m_pAIScene = nullptr;
	Importer							m_Importer;
	MODELTYPE						m_eType = MODELTYPE_END;

	_uint								m_iNumMeshes = 0;
	vector<class CMesh*>			m_vecMesh;
	typedef vector<class CMesh*>	MESHES;

	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>		m_Materials;
	typedef vector<MODELMATERIAL>	MATERIALS;

public:
	HRESULT							Ready_MeshContainers();
	HRESULT							Ready_Materials(const char* pModelFIlePath);

public:
	static CModel*					Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MODELTYPE eType, const char* pModelFilePath);
	virtual CComponent*			Clone(void* pArg = nullptr) override;
	virtual void						Free() override;
};

END