#include "stdafx.h"
#include "..\Public\Hat_CrabbyHat.h"
#include "GameInstance.h"
#include "Sigrid.h"
#include "Bone.h"

CHat_CrabbyHat::CHat_CrabbyHat(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CHat_CrabbyHat::CHat_CrabbyHat(const CHat_CrabbyHat & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHat_CrabbyHat::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CHat_CrabbyHat::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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

void CHat_CrabbyHat::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CHat_CrabbyHat::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_matrix	matSocket = m_pModelCom->Get_PivotMatrix() * m_pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_matSocketPivot);

	matSocket = matSocket * XMLoadFloat4x4(&m_pPlayer->Get_WorldMatrix());
	
	m_matSocket = matSocket;

	if (m_pRendererCom != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CHat_CrabbyHat::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"", 1);
	}

	return S_OK;
}

HRESULT CHat_CrabbyHat::Render_ShadowDepth()
{
	FAILED_CHECK_RETURN(__super::Render_ShadowDepth(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource_LightDepth(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, L"", 4);

	return S_OK;
}

HRESULT CHat_CrabbyHat::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"", 7);
	}

	return S_OK;
}

void CHat_CrabbyHat::ImGui_RenderProperty()
{
}

HRESULT CHat_CrabbyHat::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Hat_CrabbyHat", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CHat_CrabbyHat::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_Matrix(L"g_matSocket", &m_matSocket);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CHat_CrabbyHat::SetUp_ShaderResource_LightDepth()
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

HRESULT CHat_CrabbyHat::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matReflectView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	Safe_Release(pGameInstance);

	return S_OK;
}

CHat_CrabbyHat * CHat_CrabbyHat::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CHat_CrabbyHat*		pInstance = new CHat_CrabbyHat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CHat_CrabbyHat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHat_CrabbyHat::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CHat_CrabbyHat*		pInstance = new CHat_CrabbyHat(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CHat_CrabbyHat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat_CrabbyHat::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
