#include "stdafx.h"
#include "..\Public\SkyBox.h"
#include "GameInstance.h"

CSkyBox::CSkyBox(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSkyBox::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 15.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CSkyBox::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CSkyBox::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMLoadFloat4(&CGameInstance::GetInstance()->Get_CameraPosition()));

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSkyBox::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Shader_VtxTex", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_SkyBox", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Sphere", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);

	return S_OK;
}

HRESULT CSkyBox::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld");

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	m_pShaderCom->Set_Matrix(L"g_matView", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	return S_OK;
}

CSkyBox * CSkyBox::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CSkyBox*		pInstance = new CSkyBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkyBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkyBox::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CSkyBox*		pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CSkyBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkyBox::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
