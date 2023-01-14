#include "stdafx.h"
#include "..\Public\MainIsland.h"
#include "GameInstance.h"

CMainIsland::CMainIsland(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CMainIsland::CMainIsland(const CMainIsland & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMainIsland::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CMainIsland::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(180.f, 220.f, 120.f, 1.f));

	return S_OK;
}

void CMainIsland::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
}

void CMainIsland::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	_float4		vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	vPos.y = 0.f;

	if (nullptr != m_pRendererCom &&
		true == CGameInstance::GetInstance()->IsInFrustum_World(vPos, 200.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMainIsland::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		//if (m_pModelCom->IsInFrustum(i, XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix())) == false)
		//	continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

pair<_bool, _float3> CMainIsland::Picking_Mesh()
{
	_vector	vCamPos = CGameInstance::GetInstance()->Get_CameraPosition();
	_vector	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	vPos = XMVectorSetY(vPos, 10.f);

	_float		fDist = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	if (fDist > 100.f)
		return pair<_bool, _float3>(false, _float3(0.f, 0.f, 0.f));

	return m_pModelCom->Picking(g_hWnd, m_pTransformCom);
}

HRESULT CMainIsland::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_MainIsland", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CMainIsland::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	Safe_Release(pGameInstance);

	return S_OK;
}

CMainIsland * CMainIsland::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CMainIsland*		pInstance = new CMainIsland(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Createe : CMainIsland");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMainIsland::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CMainIsland*		pInstance = new CMainIsland(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CMainIsland");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainIsland::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
