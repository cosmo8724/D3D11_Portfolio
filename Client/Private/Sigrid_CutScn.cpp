#include "stdafx.h"
#include "..\Public\Sigrid_CutScn.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "SigridCutScn_State.h"

CSigrid_CutScn::CSigrid_CutScn(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CSigrid_CutScn::CSigrid_CutScn(const CSigrid_CutScn & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSigrid_CutScn::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSigrid_CutScn::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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
	 
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-65.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(647.f, 270.654f, 238.660f, 1.f));

	m_pSigridState = CSigridCutScn_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom);
	NULL_CHECK_RETURN(m_pSigridState, E_FAIL);

	return S_OK;
}

void CSigrid_CutScn::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pSigridState->Tick(dTimeDelta);
	//m_pStateMachineCom->Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta, CModel::LERP_BEGIN);
}

void CSigrid_CutScn::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pSigridState->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSigrid_CutScn::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones", 1);
	}

	return S_OK;
}

HRESULT CSigrid_CutScn::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Model_Sigrid_CutScene", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CSigrid_CutScn::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_RawValue(L"g_WinSize", &_float2((_float)g_iWinSizeX, (_float)g_iWinSizeY), sizeof(_float2));

	Safe_Release(pGameInstance);

	return S_OK;
}

CSigrid_CutScn * CSigrid_CutScn::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CSigrid_CutScn*		pInstance = new CSigrid_CutScn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSigrid_CutScn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSigrid_CutScn::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CSigrid_CutScn*		pInstance = new CSigrid_CutScn(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CSigrid_CutScn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSigrid_CutScn::Free()
{
	__super::Free();

	Safe_Release(m_pSigridState);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
