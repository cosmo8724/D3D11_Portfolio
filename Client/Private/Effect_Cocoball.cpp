#include "stdafx.h"
#include "..\Public\Effect_Cocoball.h"
#include "GameInstance.h"
#include "Sigrid.h"
#include "Trail_Cocoball.h"

CEffect_Cocoball::CEffect_Cocoball(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CEffect_Mesh(pDevice, pContext)
{
}

CEffect_Cocoball::CEffect_Cocoball(const CEffect_Cocoball & rhs)
	: CEffect_Mesh(rhs)
{
}

HRESULT CEffect_Cocoball::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Cocoball::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	m_iWidthCount = 4;
	m_iHeightCount = 4;
	m_iWidthFrame = 0;
	m_iHeightFrame = 2;

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	if (nullptr != pArg)
		memcpy(&TransformDesc, pArg, sizeof(CTransform::TRANSFORMDESC));
	else
	{
		TransformDesc.dSpeedPerSec = 50.0;
		TransformDesc.dRotationPerSec = (_double)XMConvertToRadians(90.f);
	}

	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, &TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->back());
	m_pTrail = dynamic_cast<CTrail_Cocoball*>(CGameInstance::GetInstance()->Clone_GameObjectReturnPtr(LEVEL_TESTSTAGE, L"Layer_Effect", L"Prototype_GameObject_Effect_Trail_Cocoball"));

	return S_OK;
}

void CEffect_Cocoball::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bDead == true)
		return;

	if (m_bScaleFix == false)
		m_bScaleFix = true;

	/* Move */
	if (m_bFire == false)
	{
		m_fAlpha += (_float)dTimeDelta * 0.3f;

		if (m_fAlpha > 1.f)
		{
			m_fAlpha = 1.f;
			m_bFire = true;
			m_pTrail->Set_Active(true);
			CGameInstance::GetInstance()->Play_Sound(L"SE_SU0004_Leviathan_Att_Waterball_Cocoball.wav", g_fSFXVolume, false, false);
		}
	}
	else
	{
		_double	dSpeed = m_pTransformCom->Get_Speed();

		if (m_bTurn == false)
		{
			//m_dLifeTime += dTimeDelta;
			dSpeed -= dTimeDelta * 40.0;


			m_pTransformCom->Go_Straight(dTimeDelta);

			if (dSpeed < 10.0)
			{
				m_bTurn = true;
				m_dLifeTime = 0.0;
				dSpeed = 10.0;
			}

			m_pTransformCom->Set_Speed(dSpeed);
		}
		else
		{
			m_dLifeTime += dTimeDelta * 0.1;
			dSpeed += dTimeDelta * 40.0;

			_float4	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_float4	vDir = m_pPlayer->Get_Position() - m_pTransformCom->Get_State(CTransform::STATE_TRANS);
			
			if (m_dLifeTime > 1.0)
				m_dLifeTime = 1.0;

			m_vDir = _float4::Lerp(vLook, vDir, (_float)m_dLifeTime);

			m_pTransformCom->Go_DirectionWithY(m_vDir, dTimeDelta);
			m_pTransformCom->Set_Speed(dSpeed);
		}
	}

	/* Sprite */
	m_dTimeDelta += dTimeDelta;

	if (m_bFinish == false && m_dTimeDelta > m_dSpeed)
	{
		m_dTimeDelta -= m_dSpeed;
		m_iWidthFrame--;

		if (m_iWidthFrame < 0)
		{
			m_iHeightFrame--;

			if (m_iHeightFrame < 0)
			{
				m_iHeightFrame = 2;
				m_iWidthFrame = 0;
				/*m_bFinish = true;*/
			}
			else
				m_iWidthFrame = m_iWidthCount - 1;
		}
	}

	m_pSphereCol->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
	m_pTrail->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
}

void CEffect_Cocoball::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_pPlayer->Collision_Body(m_pSphereCol))
	{
		m_pPlayer->Collision_Event(nullptr);
		m_bDead = true;
	}

	_float4	vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	if (vPos.y < 0.5f)
		m_bDead = true;

	if (m_bDead == true)
	{
		m_pTrail->Set_Dead();
		return;
	}

	if (m_pRendererCom != nullptr  && m_bScaleFix == true)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		//m_pRendererCom->Add_DebugRenderGroup(m_pSphereCol);
	}
}

HRESULT CEffect_Cocoball::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	m_pModelCom->Render(m_pShaderCom, 0, L"", 10);

	return S_OK;
}

void CEffect_Cocoball::ImGui_RenderProperty()
{
	ImGui::InputDouble("Sprite Speed", &m_dSpeed, 0.01, 0.05, "%.3f");

	if (ImGui::Button("RePlay"))
	{
		m_bFinish = false;
		m_iWidthFrame = 0;
		m_iHeightFrame = 2;
		m_dTimeDelta = 0.0;
	}
}

HRESULT CEffect_Cocoball::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Texture_Effect_Cocoball", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Effect_Cocoball", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.75f, 0.75f, 0.75f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_SphereCol", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	return S_OK;
}

HRESULT CEffect_Cocoball::SetUp_ShaderResource()
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
	m_pShaderCom->Set_RawValue(L"g_fFadeAlpha", &m_fAlpha, sizeof(_float));

	Safe_Release(pGameInstance);

	return S_OK;
}

CEffect_Cocoball * CEffect_Cocoball::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CEffect_Cocoball*		pInstance = new CEffect_Cocoball(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Cocoball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Cocoball::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CEffect_Cocoball*		pInstance = new CEffect_Cocoball(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Cocoball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Cocoball::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCol);
}
