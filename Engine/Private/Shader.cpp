#include "..\Public\Shader.h"

CShader::CShader(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
	: CComponent(rhs)
{
}

HRESULT CShader::Initialize_Prototype(const wstring & wstrShderFilePath)
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(), E_FAIL);

	return S_OK;
}

CShader * CShader::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring & wstrShaderFilPath)
{
	CShader*		pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(wstrShaderFilPath)))
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
}
