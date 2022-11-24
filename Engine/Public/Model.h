#pragma once
#include "Component.h"

BEGIN(Engine)

class CModel final : public CComponent
{
public:
	enum MODELTYPE { MODEL_NONANIM, MODEL_ANIM, MODELTYPE_END };

private:
	CModel(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT					Initialize_Prototype(MODELTYPE eType, const char* pModelFilePath);
	virtual HRESULT					Initialize(void* pArg) override;

public:
	const aiScene*					m_pAIScene = nullptr;
	Importer							m_Importer;
	MODELTYPE						m_eType = MODELTYPE_END;

	//vector<class CMesh*>			m_vecMesh;
	//typedef vector<class CMesh*>	MESHES;

public:
	static CModel*					Create(DEVICE pDevice, DEVICE_CONTEXT pContext, MODELTYPE eType, const char* pModelFilePath);
	virtual CComponent*			Clone(void* pArg = nullptr) override;
	virtual void						Free() override;
};

END