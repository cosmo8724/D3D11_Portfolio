#include "stdafx.h"
#include "..\Public\Grapple_Hang.h"
#include "GameInstance.h"
#include "Static_Camera.h"
#include "Sigrid.h"

CGrapple_Hang::CGrapple_Hang(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CGrapple_Hang::CGrapple_Hang(const CGrapple_Hang & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrapple_Hang::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CGrapple_Hang::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(180.f, 10.f, 283.5f, 1.f));

	return S_OK;
}

void CGrapple_Hang::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pRangeCol->Update(m_pTransformCom->Get_WorldMatrix());
	m_pGrapplePointCol->Update(m_pTransformCom->Get_WorldMatrix());

	CStatic_Camera*	pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	CSigrid*				pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->front());

	_vector	vRayPos, vRayDir;
	_float		fDist = 0.f;

	vRayPos = XMLoadFloat4x4(&pCamera->Get_WorldMatrix()).r[3];
	vRayDir = XMVector3Normalize(XMLoadFloat4x4(&pCamera->Get_WorldMatrix()).r[2]);

	if (m_pRangeCol->Collision_Ray(vRayPos, vRayDir, fDist) && fDist < 30.f && fDist > 5.f)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_E) && pPlayer->Is_GrappleHang() == false)
		{
			_vector		vPlayerPos = XMLoadFloat4x4(&pPlayer->Get_WorldMatrix()).r[3];
			_vector		vPlayerLook = XMLoadFloat4x4(&pPlayer->Get_WorldMatrix()).r[2];
			vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
			_vector		vDir = XMVector3Normalize(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_TRANS));
			_vector		vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			// 위 아래 각도 구해야함
			_float			fAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vPlayerLook), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_TRANS) - vPlayerPos))));
			_float			fTest = XMConvertToDegrees(fAngle);

			if (fAngle < XMConvertToRadians(25.f))
				fAngle = 0.f;
			else if (fAngle > XMConvertToRadians(25.f) && fAngle < XMConvertToRadians(75.f))
				fAngle = XMConvertToRadians(45.f);
			else
				fAngle = XMConvertToRadians(90.f);

			if (XMVectorGetY(vPlayerPos) > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_TRANS)))
				fAngle *= -1.f;

			_vector		vHangPos = XMVector3TransformCoord(XMVectorSetY(m_pGrapplePointCol->Get_ColliderPos(), -1.85f), m_pTransformCom->Get_WorldMatrix());

			/* Player -> Fire_Slow Animation */
			if (0.f <= XMVectorGetX(XMVector3Dot(vDir, vLook)))
				pPlayer->Set_SnapGrappleSlow(fAngle, 0.f, vHangPos, m_pTransformCom->Get_WorldMatrix());

			/* Player -> Fire_Fast Animation */
			else
				pPlayer->Set_SnapGrappleFast(fAngle, 0.f, vHangPos, m_pTransformCom->Get_WorldMatrix());

			pPlayer->Set_GrappleHang();
		}
	}
}

void CGrapple_Hang::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom )
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_REFLECT, this);

		if (true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		//m_pRendererCom->Add_DebugRenderGroup(m_pRangeCol);
		//m_pRendererCom->Add_DebugRenderGroup(m_pGrapplePointCol);
	}
}

HRESULT CGrapple_Hang::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i);
	}

	return S_OK;
}

HRESULT CGrapple_Hang::Render_Reflect()
{
	FAILED_CHECK_RETURN(__super::Render_Reflect(), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_ShaderResource_Reflect(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"", 7);
	}

	return S_OK;
}

HRESULT CGrapple_Hang::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Grapple_Hang", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(8.f, 8.f, 8.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Range", (CComponent**)&m_pRangeCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, -0.55f, 0.25f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_GrapplePoint", (CComponent**)&m_pGrapplePointCol, this, &ColliderDesc), E_FAIL);

	return S_OK;
}

HRESULT CGrapple_Hang::SetUp_ShaderResource()
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

HRESULT CGrapple_Hang::SetUp_ShaderResource_Reflect()
{
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	FAILED_CHECK_RETURN(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, L"g_matWorld"), E_FAIL);
	m_pShaderCom->Set_Matrix(L"g_matReflectView", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_REFLECTVIEW));
	m_pShaderCom->Set_Matrix(L"g_matProj", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	m_pShaderCom->Set_RawValue(L"g_vClipPlane", &pGameInstance->Get_ClipPlane(CPipeLine::CLIPPLANE_REFLECT), sizeof(_float4));

	Safe_Release(pGameInstance);

	return S_OK;
}

CGrapple_Hang * CGrapple_Hang::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CGrapple_Hang*		pInstance = new CGrapple_Hang(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CGrapple_Hang");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGrapple_Hang::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CGrapple_Hang*		pInstance = new CGrapple_Hang(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CGrapple_Hang");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrapple_Hang::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pGrapplePointCol);
}
