#include "stdafx.h"
#include "..\Public\Shader.h"
#include "GameUtility.h"

CShader::CShader(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_vecInputLayout(rhs.m_vecInputLayout)
	, m_iNumPasses(rhs.m_iNumPasses)
	, m_eType(rhs.m_eType)
	, m_pElements(rhs.m_pElements)
	, m_iElementCnt(rhs.m_iElementCnt)
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_vecInputLayout)
		Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const wstring & wstrShderFilePath, DECLARATIONTYPE eType, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements)
{
	m_wstrFilePath = wstrShderFilePath;
	m_eType = eType;
	m_pElements = pElements;
	m_iElementCnt = iNumElements;

	_uint	iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	FAILED_CHECK_RETURN(D3DX11CompileEffectFromFile(wstrShderFilePath.c_str(),
																	nullptr,
																	D3D_COMPILE_STANDARD_FILE_INCLUDE,
																	iHlslFlag,
																	0,
																	m_pDevice,
																	&m_pEffect,
																	nullptr), E_FAIL);

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{
		ID3D11InputLayout*	pInputLayout = nullptr;

		ID3DX11EffectPass*	pPass = pTechnique->GetPassByIndex(i);
		NULL_CHECK_RETURN(pPass, E_FAIL);

		D3DX11_PASS_DESC	PassDesc;
		pPass->GetDesc(&PassDesc);

		FAILED_CHECK_RETURN(
			m_pDevice->CreateInputLayout(pElements,
				iNumElements,
				PassDesc.pIAInputSignature,
				PassDesc.IAInputSignatureSize,
				&pInputLayout), E_FAIL);

		m_vecInputLayout.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(CGameObject * pOwner, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pOwner, pArg), E_FAIL);

	return S_OK;
}

void CShader::ImGui_RenderProperty()
{
	if (ImGui::Button("ReCompile Shader"))
		ReCompile();
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	NULL_CHECK_RETURN(m_pContext, E_FAIL);
	assert(iPassIndex < m_iNumPasses);

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);
	NULL_CHECK_RETURN(pTechnique, E_FAIL);

	ID3DX11EffectPass*			pPass = pTechnique->GetPassByIndex(iPassIndex);
	NULL_CHECK_RETURN(pPass, E_FAIL);

	pPass->Apply(0, m_pContext);

	m_pContext->IASetInputLayout(m_vecInputLayout[iPassIndex]);

	return S_OK;
}

HRESULT CShader::ReCompile()
{
	for (auto& pInputLayout : m_vecInputLayout)
		Safe_Release(pInputLayout);

	m_vecInputLayout.clear();

	Safe_Release(m_pEffect);

	_uint	iHlslFlag = 0;

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	FAILED_CHECK_RETURN(D3DX11CompileEffectFromFile(m_wstrFilePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iHlslFlag,
		0,
		m_pDevice,
		&m_pEffect,
		nullptr), E_FAIL);

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC	TechniqueDesc;
	pTechnique->GetDesc(&TechniqueDesc);

	m_iNumPasses = TechniqueDesc.Passes;

	for (_uint i = 0; i < m_iNumPasses; ++i)
	{
		ID3D11InputLayout*	pInputLayout = nullptr;

		ID3DX11EffectPass*	pPass = pTechnique->GetPassByIndex(i);
		NULL_CHECK_RETURN(pPass, E_FAIL);

		D3DX11_PASS_DESC	PassDesc;
		pPass->GetDesc(&PassDesc);

		FAILED_CHECK_RETURN(
			m_pDevice->CreateInputLayout(m_pElements,
				m_iElementCnt,
				PassDesc.pIAInputSignature,
				PassDesc.IAInputSignatureSize,
				&pInputLayout), E_FAIL);

		m_vecInputLayout.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Set_RawValue(const wstring & wstrConstantName, const void * pData, _uint iLength)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	char	szConstantName[MAX_PATH];
	CGameUtility::wctc(wstrConstantName.c_str(), szConstantName);

	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(szConstantName);
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CShader::Set_Matrix(const wstring & wstrConstantName, const _float4x4 * pMatrix)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	char	szConstantName[MAX_PATH];
	CGameUtility::wctc(wstrConstantName.c_str(), szConstantName);

	ID3DX11EffectMatrixVariable*	pVariable = m_pEffect->GetVariableByName(szConstantName)->AsMatrix();
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	return pVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CShader::Set_MatrixArray(const wstring & wstrConstantName, const _float4x4 * pMatrix, _uint iNumMatrices)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	char	szConstantName[MAX_PATH];
	CGameUtility::wctc(wstrConstantName.c_str(), szConstantName);

	ID3DX11EffectMatrixVariable*	pVariable = m_pEffect->GetVariableByName(szConstantName)->AsMatrix();
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	return pVariable->SetMatrixArray((_float*)pMatrix, 0, iNumMatrices);
}

HRESULT CShader::Set_ShaderResourceView(const wstring & wstrConstantName, ID3D11ShaderResourceView * pSRV)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	char	szConstantName[MAX_PATH];
	CGameUtility::wctc(wstrConstantName.c_str(), szConstantName);

	ID3DX11EffectShaderResourceVariable*	pVariable = m_pEffect->GetVariableByName(szConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);

	return pVariable->SetResource(pSRV);
}

HRESULT CShader::Set_ShaderResourceViewArray(const wstring & wstrConstantName, ID3D11ShaderResourceView ** ppSRV, _uint iNumTextures)
{
	NULL_CHECK_RETURN(m_pEffect, E_FAIL);

	char	szConstantName[MAX_PATH];
	CGameUtility::wctc(wstrConstantName.c_str(), szConstantName);

	ID3DX11EffectShaderResourceVariable*	pVariable = m_pEffect->GetVariableByName(szConstantName)->AsShaderResource();
	NULL_CHECK_RETURN(pVariable, E_FAIL);
	
	return pVariable->SetResourceArray(ppSRV, 0, iNumTextures);
}

CShader * CShader::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrShaderFilPath, DECLARATIONTYPE eType, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements)
{
	CShader*		pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrShaderFilPath, eType, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Create : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CShader::Clone(CGameObject * pOwner, void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pOwner, pArg)))
	{
		MSG_BOX("Failed to Clone : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	for (auto& pInputLayout : m_vecInputLayout)
		Safe_Release(pInputLayout);

	m_vecInputLayout.clear();

	Safe_Release(m_pEffect);
}
