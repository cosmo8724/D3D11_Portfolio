#include "stdafx.h"
#include "..\Public\SkyBox.h"
#include "GameInstance.h"
#include "Sigrid.h"

CSkyBox::CSkyBox(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox & rhs)
	: CGameObject(rhs)
	, m_vLightDiffuse(rhs.m_vLightDiffuse)
	, m_bLightDecrease(rhs.m_bLightDecrease)
	, m_bLightning(rhs.m_bLightning)
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

	if (m_bLightning == false)
		CGameInstance::GetInstance()->Get_LightDiffuse(0) = m_vLightDiffuse;
	else
		CGameInstance::GetInstance()->Get_LightDiffuse(1) = m_vLightDiffuse;

	if (m_bLightDecrease == true)
	{
		if (m_bLightning == false)
		{
			CGameInstance::GetInstance()->Set_LightState(0, true);
			CGameInstance::GetInstance()->Set_LightState(1, false);

			if (m_vLightDiffuse.x > 0.05f)
			{
				m_vLightDiffuse.x -= 0.002f;
				m_vLightDiffuse.y -= 0.002f;
				m_vLightDiffuse.z -= 0.002f;
			}
			else
			{
				m_vLightDiffuse.x = 0.05f;
				m_vLightDiffuse.y = 0.05f;
				m_vLightDiffuse.z = 0.05f;
			}
		}
		else
		{
			CGameInstance::GetInstance()->Set_LightState(0, false);
			CGameInstance::GetInstance()->Set_LightState(1, true);

			m_dCurLightningTime += dTimeDelta;

			if (m_dCurLightningTime > m_dMaxLightningTime)
			{
				if (m_vLightDiffuse.x > 0.05f)
				{
					m_vLightDiffuse.x -= 0.01f;
					m_vLightDiffuse.y -= 0.008f;
					m_vLightDiffuse.z -= 0.002f;
				}
				else
				{
					m_vLightDiffuse = { 0.05f, 0.05f, 0.05f, 1.f };
					m_bLightning = false;
				}
			}
		}
	}
	else
	{
		CGameInstance::GetInstance()->Set_LightState(0, true);

		m_vLightDiffuse.x += 0.002f;
		m_vLightDiffuse.y += 0.002f;
		m_vLightDiffuse.z += 0.002f;

		if (m_vLightDiffuse.x > 1.f)
		{
			m_vLightDiffuse.x = 1.f;
			m_vLightDiffuse.y = 1.f;
			m_vLightDiffuse.z = 1.f;
		}
	}
}

void CSkyBox::Late_Tick(_double dTimeDelta)
{
 	__super::Late_Tick(dTimeDelta);

	_vector	vCamPos = CGameInstance::GetInstance()->Get_CameraPosition();

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, vCamPos);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this, true);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
	}
}

HRESULT CSkyBox::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSkyBox::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	m_pShaderCom->Begin(9);

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
	m_pShaderCom->Set_RawValue(L"g_vDiffuseColor", &m_vLightDiffuse, sizeof(_float4));

	return S_OK;
}

HRESULT CSkyBox::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	_float4x4		matWorld = m_pTransformCom->Get_WorldMatrix();
	/*matWorld._42 *= -1.f;*/

	if (CGameInstance::GetInstance()->Get_CurLevelIndex() == LEVEL_TESTSTAGE)
	{
		_matrix		matPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back())->Get_WorldMatrix();

		memcpy(&matWorld._41, &matPlayer.r[3], sizeof(_float4));
		matWorld._42 *= -1.f;
	}

	m_pShaderCom->Set_Matrix(L"g_matWorld", &matWorld);

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_Texture");

	m_pShaderCom->Set_Matrix(L"g_matReflectView", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_RawValue(L"g_vDiffuseColor", &m_vLightDiffuse, sizeof(_float4));
	//m_pShaderCom->Set_RawValue(L"g_vClipPlane", &CGameInstance::GetInstance()->Get_ClipPlane(CPipeLine::CLIPPLANE_REFRACT), sizeof(_float4));

	return S_OK;
}

void CSkyBox::Lightning()
{
	
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
