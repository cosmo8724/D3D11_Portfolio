#include "stdafx.h"
#include "..\Public\Grapple_Launcher.h"
#include "GameInstance.h"
#include "Static_Camera.h"
#include "Sigrid.h"

CGrapple_Launcher::CGrapple_Launcher(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CGrapple_Launcher::CGrapple_Launcher(const CGrapple_Launcher & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGrapple_Launcher::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CGrapple_Launcher::Initialize(const wstring & wstrPrototypeTag, void * pArg)
{
	FAILED_CHECK_RETURN(__super::Initialize(wstrPrototypeTag, pArg), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(202.f, 10.f, 283.5f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-80.f));

	return S_OK;
}

void CGrapple_Launcher::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pRangeCol->Update(m_pTransformCom->Get_WorldMatrix());
	m_pGrapplePointCol->Update(m_pTransformCom->Get_WorldMatrix());

	_vector	vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector	vLookAxis = XMVectorSetY(vLook, 0.f);

	m_fLaunchAngle = acosf(XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(vLookAxis))));

	if (m_fLaunchAngle < XMConvertToRadians(25.f))
		m_fLaunchAngle = 0.f;
	else if (m_fLaunchAngle > XMConvertToRadians(25.f) && m_fLaunchAngle < XMConvertToRadians(75.f))
		m_fLaunchAngle = XMConvertToRadians(45.f);
	else
		m_fLaunchAngle = XMConvertToRadians(90.f);

	if (0.f > XMVectorGetX(XMVector3Dot(XMVector3Normalize(vLook), XMVectorSet(0.f, 1.f, 0.f, 0.f))))
		m_fLaunchAngle *= -1.f;

	CStatic_Camera*	pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	CSigrid*				pPlayer = dynamic_cast<CSigrid*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Player")->front());

	_vector	vRayPos, vRayDir;
	_float		fDist = 0.f;

	vRayPos = XMLoadFloat4x4(&pCamera->Get_WorldMatrix()).r[3];
	vRayDir = XMVector3Normalize(XMLoadFloat4x4(&pCamera->Get_WorldMatrix()).r[2]);

	if (m_pRangeCol->Collision_Ray(vRayPos, vRayDir, fDist) && fDist < 30.f && fDist > 5.f)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_E) && pPlayer->Is_GrappleLauncher() == false)
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

			/* Player -> Fire_Slow Animation */
			if (0.f <= XMVectorGetX(XMVector3Dot(vDir, vLook)))
				pPlayer->Set_SnapGrappleSlow(fAngle, m_fLaunchAngle, XMVector3TransformCoord(m_pGrapplePointCol->Get_ColliderPos(), m_pTransformCom->Get_WorldMatrix()), m_pTransformCom->Get_WorldMatrix());

			/* Player -> Fire_Fast Animation */
			else
				pPlayer->Set_SnapGrappleFast(fAngle, m_fLaunchAngle, XMVector3TransformCoord(m_pGrapplePointCol->Get_ColliderPos(), m_pTransformCom->Get_WorldMatrix()), m_pTransformCom->Get_WorldMatrix());

			pPlayer->Set_GrappleLauncher();
		}
	}
}

void CGrapple_Launcher::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom &&
		true == CGameInstance::GetInstance()->IsInFrustum_World(m_pTransformCom->Get_State(CTransform::STATE_TRANS), 2.f))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CGrapple_Launcher::Render()
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

#ifdef _DEBUG
	m_pRangeCol->Render();
	m_pGrapplePointCol->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CGrapple_Launcher::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_NonAnim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Grapple_Launcher", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(8.f, 8.f, 8.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Range", (CComponent**)&m_pRangeCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, -1.f, -4.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_GrapplePoint", (CComponent**)&m_pGrapplePointCol, this, &ColliderDesc), E_FAIL);

	return S_OK;
}

HRESULT CGrapple_Launcher::SetUp_ShaderResource()
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

CGrapple_Launcher * CGrapple_Launcher::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CGrapple_Launcher*		pInstance = new CGrapple_Launcher(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CGrapple_Launcher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGrapple_Launcher::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CGrapple_Launcher*		pInstance = new CGrapple_Launcher(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CGrapple_Launcher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGrapple_Launcher::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pRangeCol);
	Safe_Release(m_pGrapplePointCol);
}
