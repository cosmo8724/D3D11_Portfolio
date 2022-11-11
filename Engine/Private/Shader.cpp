#include "..\Public\Shader.h"

CShader::CShader(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_vecInputLayout(rhs.m_vecInputLayout)
	, m_iNumPasses(rhs.m_iNumPasses)
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_vecInputLayout)
		Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const wstring & wstrShderFilePath, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements)
{
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

HRESULT CShader::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(pArg), E_FAIL);

	return S_OK;
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

CShader * CShader::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrShaderFilPath, const D3D11_INPUT_ELEMENT_DESC * pElements, const _uint iNumElements)
{
	CShader*		pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrShaderFilPath, pElements, iNumElements)))
	{
		MSG_BOX("Failed to Create : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
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
