#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstancing final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_PointInstancing(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CVIBuffer_PointInstancing(const CVIBuffer_PointInstancing& rhs);
	virtual ~CVIBuffer_PointInstancing() = default;

public:
	vector<VTXMATRIX>*		Get_InstanceInfo() { return &m_vecInstanceInfo; }

public:
	virtual HRESULT		Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT		Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual HRESULT		Tick(_double dTimeDelta);
	virtual HRESULT		Tick_Trail(_double dTimeDelta);
	virtual HRESULT		Render() override;

public:
	void					Add_Instance(_float4x4 matInfo);
	HRESULT				Bind_ShaderResource(class CShader* pShaderCom, const wstring& wstrConstantName);

private:
	_uint					m_iInitNumInstance = 0;
	vector<VTXMATRIX>	m_vecInstanceInfo;

public:
	static CVIBuffer_PointInstancing*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, _uint iNumInstance = 1);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END