#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
public:
	enum DECLARATIONTYPE { DECLARATION_VTXPOS, DECLARATION_VTXTEX, DECLARATION_VTXNORTEX, DECLARATION_VTXMODEL, DECLARATION_VTXANIMMODEL, DECLARATION_VTXPOINTINSTANCING, DECLARATION_END };

private:
	CShader(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	const DECLARATIONTYPE&		Get_DeclarationType() const { return m_eType; }
	const _uint&						Get_ElementsCnt() const { return m_iElementCnt; }
	HRESULT					Set_RawValue(const wstring& wstrConstantName, const void* pData, _uint iLength);
	HRESULT					Set_Matrix(const wstring& wstrConstantName, const _float4x4* pMatrix);
	HRESULT					Set_MatrixArray(const wstring& wstrConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT					Set_ShaderResourceView(const wstring& wstrConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT					Set_ShaderResourceViewArray(const wstring& wstrConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures);

public:
	HRESULT					Initialize_Prototype(const wstring& wstrShderFilePath, DECLARATIONTYPE eType, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements);
	virtual	HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual void				ImGui_RenderProperty() override;
	
public:
	HRESULT					Begin(_uint iPassIndex);
	HRESULT					ReCompile();

private:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<ID3D11InputLayout*>	m_vecInputLayout;
	_uint							m_iNumPasses = 0;

	DECLARATIONTYPE			m_eType = DECLARATION_END;
	const D3D11_INPUT_ELEMENT_DESC * m_pElements = nullptr;
	_uint							m_iElementCnt = 0;

public:
	static CShader*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrShaderFilPath, DECLARATIONTYPE eType, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END