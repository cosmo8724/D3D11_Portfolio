#include "stdafx.h"
#include "..\Public\Sword_Handle.h"
#include "GameInstance.h"
#include "Player.h"
#include "Bone.h"

CSword_Handle::CSword_Handle(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CWeapon(pDevice, pContext)
{
}

CSword_Handle::CSword_Handle(const CSword_Handle & rhs)
	: CWeapon(rhs)
{
}

HRESULT CSword_Handle::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSword_Handle::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CSword_Handle::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CSword_Handle::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_matrix	matSocket = m_pModelCom->Get_PivotMatrix() * m_tWeaponDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_tWeaponDesc.matPivot);

	matSocket.r[0] = XMVector3Length(m_pModelCom->Get_PivotMatrix().r[0]) * XMVector3Normalize(matSocket.r[0]);
	matSocket.r[1] = XMVector3Length(m_pModelCom->Get_PivotMatrix().r[1]) * XMVector3Normalize(matSocket.r[1]);
	matSocket.r[2] = XMVector3Length(m_pModelCom->Get_PivotMatrix().r[2]) * XMVector3Normalize(matSocket.r[2]);

	matSocket = matSocket * XMLoadFloat4x4(&m_tWeaponDesc.pTargetTransform->Get_WorldMatrix());

	XMStoreFloat4x4(&m_matSocket, matSocket);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSword_Handle::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"", 1);
	}

	return S_OK;
}

HRESULT CSword_Handle::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Handle", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CSword_Handle::SetUp_ShaderResource()
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

CSword_Handle * CSword_Handle::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CSword_Handle*		pInstance = new CSword_Handle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSword_Handle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSword_Handle::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CSword_Handle*		pInstance = new CSword_Handle(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CSword_Handle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword_Handle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
