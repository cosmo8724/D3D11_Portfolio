#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT					Initialize_Prototype(const wstring& wstrShderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements);
	virtual	HRESULT			Initialize(void* pArg) override;
	
public:
	HRESULT					Begin(_uint iPassIndex);
	HRESULT					Set_RawValue(const char* pConstantName, const void* pData, _uint iLength);
	HRESULT					Set_Matrix(const char* pConstantName, const _float4x4* pMatrix);

private:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<ID3D11InputLayout*>	m_vecInputLayout;

private:
	_uint							m_iNumPasses = 0;

public:
	static CShader*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrShaderFilPath, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END