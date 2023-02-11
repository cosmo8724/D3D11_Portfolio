#include "stdafx.h"
#include "..\Public\Hat_FuzzyEars.h"
#include "GameInstance.h"
#include "Sigrid.h"
#include "Bone.h"

CHat_FuzzyEars::CHat_FuzzyEars(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CHat_FuzzyEars::CHat_FuzzyEars(const CHat_FuzzyEars & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHat_FuzzyEars::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CHat_FuzzyEars::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	HATDESC	HatDesc;
	ZeroMemory(&HatDesc, sizeof(HATDESC));

	if (pArg != nullptr)
	{
		memcpy(&HatDesc, pArg, sizeof(HATDESC));

		m_matSocketPivot = HatDesc.matSocketPivot;
		m_pSocket = HatDesc.pSocket;
		m_pPlayerTransformCom = HatDesc.pPlayerTransformCom;
		m_pPlayer = HatDesc.pPlayer;
	}
	else
		return E_FAIL;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &HatDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(0.f, -0.125f, 0.07f, 1.f));

	return S_OK;
}

void CHat_FuzzyEars::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_vHairColor = m_pPlayer->Get_CurrentHairColor();

	if (m_vHairColor.w != 0.f)
		m_bHairMask = true;
	else
	{
		m_bHairMask = false;
		m_vHairColor = { 1.f, 1.f, 1.f, 1.f, };
	}
}

void CHat_FuzzyEars::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_matrix	matSocket = m_pModelCom->Get_PivotMatrix() * m_pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_matSocketPivot);

	matSocket = matSocket * XMLoadFloat4x4(&m_pPlayer->Get_WorldMatrix());

	m_matSocket = matSocket;

	m_pTransformCom->Set_Scale(_float3(0.9f, 0.9f, 0.9f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(0.f, -0.13f, 0.1f, 1.f));

	if (m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CHat_FuzzyEars::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"", 5);
	}

	return S_OK;
}

HRESULT CHat_FuzzyEars::Render_ShadowDepth()
{
	FAILED_CHECK_RETURN(__super::Render_ShadowDepth(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource_LightDepth(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, L"", 4);

	return S_OK;
}

HRESULT CHat_FuzzyEars::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"", 9);
	}

	return S_OK;
}

void CHat_FuzzyEars::ImGui_RenderProperty()
{
}

HRESULT CHat_FuzzyEars::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hat_FuzzyEars", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Skin_HairMask", L"Com_HairMask", (CComponent**)&m_pHairMaskTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CHat_FuzzyEars::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_Matrix(L"g_matSocket", &m_matSocket);
	m_pHairMaskTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_MaskTexture");

	m_pShaderCom->Set_RawValue(L"g_bHairMask", &m_bHairMask, sizeof(_bool));
	m_pShaderCom->Set_RawValue(L"g_vHairColor", &m_vHairColor, sizeof(_float4));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHat_FuzzyEars::SetUp_ShaderResource_LightDepth()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);

	const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);

	//_float4	vLightEye = pLightDesc->vPosition;
	//_float4	vLightAt = XMLoadFloat4(&vLightEye) + XMVector3Normalize(XMLoadFloat4(&pLightDesc->vDirection));
	//_float4	vLightUp = { 0.f, 1.f, 0.f, 0.f };

	_float4		vLightEye = _float4(-500.f, 1000.f, -500.f, 1.f);
	_float4		vLightAt = _float4(1000.f, 0.f, 1000.f, 1.f);
	_float4		vLightUp = _float4(0.f, 1.f, 0.f, 0.f);

	_float4x4	matLightView = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4	matLightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), (_float)g_iWinSizeX / (_float)g_iWinSizeY, 0.1f, 3000.f);

	m_pShaderCom->Set_Matrix(L"g_matView", &matLightView);
	//m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_Matrix(L"g_matProj", &matLightProj);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHat_FuzzyEars::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matReflectView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_Matrix(L"g_matSocket", &m_matSocket);
	m_pHairMaskTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_MaskTexture");

	m_pShaderCom->Set_RawValue(L"g_bHairMask", &m_bHairMask, sizeof(_bool));
	m_pShaderCom->Set_RawValue(L"g_vHairColor", &m_vHairColor, sizeof(_float4));

	Safe_Release(pGameInstance);

	return S_OK;
}

CHat_FuzzyEars * CHat_FuzzyEars::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CHat_FuzzyEars*		pInstance = new CHat_FuzzyEars(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHat_FuzzyEars");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHat_FuzzyEars::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CHat_FuzzyEars*		pInstance = new CHat_FuzzyEars(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CHat_FuzzyEars");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat_FuzzyEars::Free()
{
	__super::Free();

	Safe_Release(m_pHairMaskTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
