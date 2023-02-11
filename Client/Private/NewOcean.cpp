#include "stdafx.h"
#include "..\Public\NewOcean.h"
#include "GameInstance.h"

CNewOcean::CNewOcean(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CNewOcean::CNewOcean(const CNewOcean & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNewOcean::Initialize_Prototype()
{
	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CNewOcean::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CNewOcean::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_dTimeDelta += dTimeDelta;
}

void CNewOcean::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	Update_ReflectionCam();
	Update_ClipPlane();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CNewOcean::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CNewOcean::ImGui_RenderProperty()
{
	ImGui::InputFloat("Water Level", &m_fWaterLevel, 0.5f, 1.f);
	ImGui::InputFloat("Water Speed", &m_fWaterSpeed, 0.1f, 0.5f);
	ImGui::InputFloat("Wave Length", &m_fWaveLength, 0.1f, 0.5f);
	ImGui::InputFloat("Wave Height", &m_fWaveHeight, 0.1f, 0.5f);
	ImGui::InputFloat("Wind Force", &m_fWindForce, 1.f, 5.f);
	ImGui::InputFloat("Specular Per Turb", &m_fSpecularPerturb, 0.5f, 1.f);
	ImGui::InputFloat("Specular Power", &m_fSpecularPower, 1.f, 5.f);
	ImGui::InputFloat("Dull Blend Factor", &m_fDullBlendFactor, 0.1f, 0.5f);

	ImGui::ColorPicker3("Water Color", (_float*)&m_vWaterColor, ImGuiColorEditFlags_None);
}

HRESULT CNewOcean::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Shader_NewOcean", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_VIBuffer_Point_Instancing", L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);

	//FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_Ocean", L"Com_Texture", (CComponent**)&m_pTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_NewOcean_Foam", L"Com_Texture_Foam", (CComponent**)&m_pFoamTextureCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_LOGO, L"Prototype_Component_Texture_NewOcean_Height", L"Com_Texture_Height", (CComponent**)&m_pHeightTextureCom, this), E_FAIL);

	return S_OK;
}

HRESULT CNewOcean::SetUp_ShaderResource()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_Matrix(L"g_matReflectionView", &m_matReflectionView);

	_float4x4		matTemp = XMMatrixRotationZ(XMConvertToRadians(90.f));
	m_pShaderCom->Set_Matrix(L"g_matWinDirection", &matTemp);

	const LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	_vector	vLightDir = XMVector3Normalize(XMLoadFloat4(&pLightDesc->vDirection));
	m_pShaderCom->Set_RawValue(L"g_vLightDir", &vLightDir, sizeof(_float4));

	_vector	vCamPosition = pGameInstance->Get_CameraPosition();
	m_pShaderCom->Set_RawValue(L"g_vCamPosition", &vCamPosition, sizeof(_float4));

	m_pShaderCom->Set_RawValue(L"g_vWaterColor", &m_vWaterColor, sizeof(_float3));
	m_pShaderCom->Set_RawValue(L"g_fWaveLength", &m_fWaveLength, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fWaveHeight", &m_fWaveHeight, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fWindForce", &m_fWindForce, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fSpecularPerturb", &m_fSpecularPerturb, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fSpecularPower", &m_fSpecularPower, sizeof(_float));
	m_pShaderCom->Set_RawValue(L"g_fDullBlendFactor", &m_fDullBlendFactor, sizeof(_float));

	_float		fTime = (_float)m_dTimeDelta / m_fWaterSpeed;
	m_pShaderCom->Set_RawValue(L"g_fTime", &fTime, sizeof(_float));

	//m_pTextureCom->Bind_ShaderResources(m_pShaderCom, L"g_RefractionTexture");
	m_pShaderCom->Set_ShaderResourceView(L"g_RefractionTexture", pGameInstance->Get_ShaderResourceView(L"Target_Diffuse"));
	m_pShaderCom->Set_ShaderResourceView(L"g_ReflectionTexture", pGameInstance->Get_ShaderResourceView(L"Target_Reflect"));
	m_pHeightTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_WaterBumpTexture");
	m_pFoamTextureCom->Bind_ShaderResource(m_pShaderCom, L"g_DiffuseTexture");

	Safe_Release(pGameInstance);

	return S_OK;
}

void CNewOcean::Update_ReflectionCam()
{
	//if (CGameInstance::GetInstance()->Get_CurLevelIndex() != LEVEL_TESTSTAGE)
	//	return;

	_vector	vCamPos, vCamLook, vTargetPos;
	_vector	vCamRight;

	//vCamPos = CGameInstance::GetInstance()->Get_CameraPosition();
	//vCamPos = XMVectorSetY(vCamPos, XMVectorGetY(vCamPos) - 2.f);
	vCamPos= CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_CAMWORLD).r[3];
	vCamLook = CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_CAMWORLD).r[2];
	vCamRight = CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_CAMWORLD).r[0];

	vTargetPos = vCamPos + vCamLook;

	_float		fReflectionCamYCoord = -XMVectorGetY(vCamPos) + 2.f * m_fWaterLevel;
	_vector	vReflectionCamPos = XMVectorSet(XMVectorGetX(vCamPos), fReflectionCamYCoord, XMVectorGetZ(vCamPos), 1.f);
	
	_float		fReflectionTargetYCoord = -XMVectorGetY(vTargetPos) +2.f * m_fWaterLevel;
	_vector	vReflectionCamTarget = XMVectorSet(XMVectorGetX(vTargetPos), fReflectionTargetYCoord, XMVectorGetZ(vTargetPos), 1.f);

	_vector	vForward = vReflectionCamTarget - vReflectionCamPos;
	//_vector	vReflectionCamUp = XMVector3Cross(vCamRight, vForward);
	_vector	vReflectionCamUp = XMVector3Cross(vCamRight, vForward);

	m_matReflectionView = XMMatrixLookAtLH(vReflectionCamPos, vReflectionCamTarget, vReflectionCamUp);

	CGameInstance::GetInstance()->Set_Transform(CPipeLine::D3DTS_REFLECTVIEW, XMLoadFloat4x4(&m_matReflectionView));
}

void CNewOcean::Update_ClipPlane()
{
	_float3	vPlaneNormalDirection = _float3(0.f, 1.f, 0.f);
	vPlaneNormalDirection.Normalize();

	_float4	vPlaneCoefficient = _float4(vPlaneNormalDirection.x, vPlaneNormalDirection.y, vPlaneNormalDirection.z, -m_fWaterLevel);
	_float4x4	matCam = m_matReflectionView * CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	_float4x4	matInvCam = matCam.Invert();
	matInvCam = matInvCam.Transpose();

	vPlaneCoefficient = XMVector4Transform(vPlaneCoefficient, matInvCam);

	m_vClipPlane[0] = vPlaneCoefficient;

	CGameInstance::GetInstance()->Set_ClipPlane(CPipeLine::CLIPPLANE_REFLECT, m_vClipPlane[0]);

	vPlaneNormalDirection = _float3(0.f, -1.f, 0.f);
	vPlaneNormalDirection.Normalize();
	vPlaneCoefficient = _float4(vPlaneNormalDirection.x, vPlaneNormalDirection.y, vPlaneNormalDirection.z, m_fWaterLevel);
	matCam = CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW) * CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	matInvCam = matCam.Invert();
	matInvCam = matInvCam.Transpose();

	vPlaneCoefficient = XMVector4Transform(vPlaneCoefficient, matInvCam);

	m_vClipPlane[1] = vPlaneCoefficient;

	CGameInstance::GetInstance()->Set_ClipPlane(CPipeLine::CLIPPLANE_REFRACT, m_vClipPlane[1]);
}

CNewOcean * CNewOcean::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CNewOcean*		pInstance = new CNewOcean(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Createe : CNewOcean");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNewOcean::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CNewOcean*		pInstance = new CNewOcean(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CNewOcean");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNewOcean::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pFoamTextureCom);
	Safe_Release(m_pHeightTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
