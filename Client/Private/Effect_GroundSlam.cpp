#include "stdafx.h"
#include "..\Public\Effect_GroundSlam.h"
#include "GameInstance.h"

CEffect_GroundSlam::CEffect_GroundSlam(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CEffect_GroundSlam::CEffect_GroundSlam(const CEffect_GroundSlam & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CEffect_GroundSlam::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_GroundSlam::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_iWidthCount = 4;
	m_iHeightCount = 4;

	return S_OK;
}

void CEffect_GroundSlam::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bScaleFix == false)
	{
		m_pTransformCom->Set_Scale(_float3(0.3f, 0.1f, 0.1f));
		_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
		vPos.y += 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);
		m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));
		m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
		m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(180.f));
		m_bScaleFix = true;
	}

	//m_pTransformCom->LookAt_NoUpDown(CGameInstance::GetInstance()->Get_CameraPosition());
	//m_pTransformCom->RotationFromNow(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));

	m_dLimitTime += dTimeDelta;
	m_fFadeAlpha -= (_float)dTimeDelta * 0.5f;

	m_dTimeDelta += dTimeDelta;

	if (m_bFinish == false && m_dTimeDelta > m_dSpeed)
	{
		m_dTimeDelta -= m_dSpeed;
		m_iWidthFrame++;

		if (m_iWidthFrame >= m_iWidthCount)
		{
			m_iHeightFrame++;

			if (m_iHeightFrame >= m_iHeightCount)
			{
				m_iHeightFrame--;
				m_bFinish = true;
				m_bDead = true;
			}
			else
				m_iWidthFrame = 0;
		}
	}
}

void CEffect_GroundSlam::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_fFadeAlpha < 0.f)
	{
		//m_bDead = true;
		//return;
	}

	if (nullptr != m_pRendererCom && m_bScaleFix == true)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_GroundSlam::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Render(m_pShaderCom, 0, L"", 6);

	return S_OK;
}

void CEffect_GroundSlam::ImGui_RenderProperty()
{
}

HRESULT CEffect_GroundSlam::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_WhiteSRC", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_GroundSlamCircle", L"Com_MaskTexture", (CComponent**)&m_pMaskTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Circle", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	return S_OK;
}

HRESULT CEffect_GroundSlam::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
	m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_MaskTexture");
	m_pShaderCom->Set_RawValue(L"g_fFadeAlpha", &m_fFadeAlpha, sizeof(_float));

	m_pShaderCom->Set_RawValue(L"g_WidthFrame", &m_iWidthFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightFrame", &m_iHeightFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_WidthCount", &m_iWidthCount, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightCount", &m_iHeightCount, sizeof(_uint));

	Safe_Release(pGameInstance);

	return S_OK;
}

CEffect_GroundSlam * CEffect_GroundSlam::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CEffect_GroundSlam*		pInstance = new CEffect_GroundSlam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_GroundSlam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_GroundSlam::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CEffect_GroundSlam*		pInstance = new CEffect_GroundSlam(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_GroundSlam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_GroundSlam::Free()
{
	__super::Free();
}
