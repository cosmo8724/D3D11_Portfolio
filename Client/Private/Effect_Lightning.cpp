#include "stdafx.h"
#include "..\Public\Effect_Lightning.h"
#include "GameInstance.h"

CEffect_Lightning::CEffect_Lightning(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CEffect_Lightning::CEffect_Lightning(const CEffect_Lightning & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CEffect_Lightning::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Lightning::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_iWidthCount = 4;
	m_iHeightCount = 5;

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CEffect_Lightning::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bDead == true)
		return;

	if (m_bScaleFix == false)
	{
		_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
		vPos.y = 110.f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, vPos);

		m_pTransformCom->RotationFromNow(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(270.f));
		m_pTransformCom->Set_Scale(_float3(50.f, 100.f, 50.f));

		srand(unsigned(time(NULL)));
		_float		fAngle = _float(rand() % 360 + 1);
		m_pTransformCom->RotationFromNow(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));

		m_bScaleFix = true;
	}

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

void CEffect_Lightning::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_bDead == true)
		return;

	if (nullptr != m_pRendererCom && m_bScaleFix == true)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

HRESULT CEffect_Lightning::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Render(m_pShaderCom, 0, L"", 10);

	return S_OK;
}

HRESULT CEffect_Lightning::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Render(m_pShaderCom, 0, L"", 11);

	return S_OK;
}

void CEffect_Lightning::ImGui_RenderProperty()
{
	ImGui::InputDouble("Sprite Speed", &m_dSpeed, 0.01, 0.05, "%.3f");

	if (ImGui::Button("RePlay"))
	{
		m_bFinish = false;
		m_iWidthFrame = 0;
		m_iHeightFrame = 0;
		m_dTimeDelta = 0.0;
	}
}

HRESULT CEffect_Lightning::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_Lighting", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	if (rand() % 2 == 0)
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Lightning_1", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);
	}
	else
	{
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Lightning_2", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);
	}

	return S_OK;
}

HRESULT CEffect_Lightning::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
	m_pShaderCom->Set_RawValue(L"g_WidthFrame", &m_iWidthFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightFrame", &m_iHeightFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_WidthCount", &m_iWidthCount, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightCount", &m_iHeightCount, sizeof(_uint));

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CEffect_Lightning::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matReflectView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");
	m_pShaderCom->Set_RawValue(L"g_WidthFrame", &m_iWidthFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightFrame", &m_iHeightFrame, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_WidthCount", &m_iWidthCount, sizeof(_uint));
	m_pShaderCom->Set_RawValue(L"g_HeightCount", &m_iHeightCount, sizeof(_uint));

	Safe_Release(pGameInstance);

	return S_OK;
}

CEffect_Lightning * CEffect_Lightning::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CEffect_Lightning*		pInstance = new CEffect_Lightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Lightning::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CEffect_Lightning*		pInstance = new CEffect_Lightning(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Lightning");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Lightning::Free()
{
	__super::Free();
}
