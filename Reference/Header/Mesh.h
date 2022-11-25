#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint&				Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	virtual HRESULT			Initialize_Prototype(aiMesh* pAIMesh);
	virtual HRESULT			Initialize(void* pArg) override;

private:
	_uint						m_iMaterialIndex = 0;

public:
	static CMesh*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, aiMesh* pAIMesh);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END