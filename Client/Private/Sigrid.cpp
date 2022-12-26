#include "stdafx.h"
#include "..\Public\Sigrid.h"
#include "GameInstance.h"
#include "GameUtility.h"
#include "Static_Camera.h"
#include "Sigrid_State.h"
#include "Ocean.h"

CSigrid::CSigrid(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
	, m_fGravity(0.02f)
	, m_fInitJumpSpeed(1.5f)
	, m_fCurJumpSpeed(0.f)
	, m_fFriction(0.1f)
	, m_fMaxDashTickCount(1.f)
	, m_fCurDashTickCount(0.f)
{
}

CSigrid::CSigrid(const CSigrid & rhs)
	: CGameObject(rhs)
	, m_fGravity(rhs.m_fGravity)
	, m_fInitJumpSpeed(rhs.m_fInitJumpSpeed)
	, m_fCurJumpSpeed(rhs.m_fCurJumpSpeed)
	, m_fFriction(rhs.m_fFriction)
	, m_fMaxDashTickCount(rhs.m_fMaxDashTickCount)
	, m_fCurDashTickCount(rhs.m_fCurDashTickCount)
{
}

HRESULT CSigrid::Initialize_Prototype()
{
	m_bHasModel = true;

	FAILED_CHECK_RETURN(__super::Initialize_Prototype(), E_FAIL);

	return S_OK;
}

HRESULT CSigrid::Initialize(const wstring & wstrPrototypeTag, void * pArg)
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

	m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMVectorSet(15.f, 0.f, 15.f, 1.f));

	/* Set Camera */
	m_pCamera = dynamic_cast<CStatic_Camera*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Camera")->back());
	m_pCamera->Set_OwnerTransform(m_pTransformCom);
	m_pCamera->Init_Position();

	/* Set Friend State class */
	m_pSigridState = CSigrid_State::Create(this, m_pStateMachineCom, m_pModelCom, m_pTransformCom, m_pCamera);
	NULL_CHECK_RETURN(m_pSigridState, E_FAIL);

	m_pModelCom->Set_CurAnimationIndex(119);

	return S_OK;
}

void CSigrid::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	SetOn_Terrain();

	m_pSigridState->Tick(dTimeDelta);
	m_pStateMachineCom->Tick(dTimeDelta);

	m_pModelCom->Play_Animation(dTimeDelta, m_eLerpType);

	m_pOBBCol->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
	m_pSphereCol->Update(m_pTransformCom->Get_WorldMatrix());

	_matrix	matNetRing = m_pModelCom->Get_BoneMatrix("NetRing DEF 1");
	m_pNetSphereCol->Update(matNetRing * m_pModelCom->Get_PivotMatrix() * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
}

void CSigrid::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	m_pSigridState->Late_Tick(dTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSigrid::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

		m_pModelCom->Render(m_pShaderCom, i, L"g_matBones");
	}

#ifdef _DEBUG
	m_pSphereCol->Render();
	m_pOBBCol->Render();
	m_pNetSphereCol->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CSigrid::SetUp_Component()
{
	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Shader_Anim", L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Model_Sigrid", L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);

	_float Xmin = (_float)SHRT_MAX, Xmax = (_float)SHRT_MIN, Ymin = (_float)SHRT_MAX, Ymax = (_float)SHRT_MIN, Zmin = (_float)SHRT_MAX, Zmax = (_float)SHRT_MIN;
	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("Sigrid_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.5f, Ymax - Ymin, (Zmax - Zmin) * 0.9f);
	ColliderDesc.vPosition = _float3(0.f, 0.9f, 0.f);
	ColliderDesc.vRotation = _float3(XMConvertToRadians(-90.f), 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_OBB", L"Com_OBB", (CComponent**)&m_pOBBCol, this, &ColliderDesc), E_FAIL);

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.vPosition = _float3(0.f, 1.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_Sphere", (CComponent**)&m_pSphereCol, this, &ColliderDesc), E_FAIL);

	FAILED_CHECK_RETURN(m_pModelCom->Check_MeshSize("NetRing_Mesh_LOD0", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax), E_FAIL);
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3((Xmax - Xmin) * 0.3f, (Ymax - Ymin) * 0.3f, (Zmax - Zmin) * 0.3f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, L"Prototype_Component_Collider_Sphere", L"Com_NetSphere", (CComponent**)&m_pNetSphereCol, this, &ColliderDesc), E_FAIL);

	FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), L"Prototype_Component_StateMachine", L"Com_StateMachine", (CComponent**)&m_pStateMachineCom, this), E_FAIL);

	return S_OK;
}

HRESULT CSigrid::SetUp_ShaderResource()
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

void CSigrid::SetOn_Terrain()
{
	_float4	vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANS);
	COcean*	pOcean = dynamic_cast<COcean*>(CGameInstance::GetInstance()->Get_CloneObjectList(LEVEL_TESTSTAGE, L"Layer_Ocean")->front());
	CVIBuffer_Terrain*		pVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(pOcean->Get_Component(L"Com_VIBuffer"));
	_uint		iNumVerticesX = pVIBuffer->Get_NumVerticesX();
	_uint		iNumVerticesZ = pVIBuffer->Get_NumVerticesZ();
	_float3*	pTerrainPos = pVIBuffer->Get_TerrainPosition();

	_uint	iIndex = (_int)vPlayerPos.z * iNumVerticesX + (_int)vPlayerPos.x;

	if (iIndex < 0 || iIndex > (iNumVerticesX - 1) * (iNumVerticesZ - 1) || vPlayerPos.x < 0.f || vPlayerPos.z < 0.f)
		vPlayerPos.y = 0.f;
	else
	{
		_float	fWidth = vPlayerPos.x - pTerrainPos[iIndex + iNumVerticesX].x;
		_float	fHeight = pTerrainPos[iIndex + iNumVerticesX].z - vPlayerPos.z;

		_vector	vPlane;

		if (fWidth > fHeight)
			vPlane = XMPlaneFromPoints(pTerrainPos[iIndex + iNumVerticesX], pTerrainPos[iIndex + iNumVerticesX + 1], pTerrainPos[iIndex + 1]);
		else
			vPlane = XMPlaneFromPoints(pTerrainPos[iIndex + iNumVerticesX], pTerrainPos[iIndex + 1], pTerrainPos[iIndex]);

		vPlayerPos.y = (XMVectorGetX(vPlane) * vPlayerPos.x * -1.f - XMVectorGetZ(vPlane) * vPlayerPos.z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
	}

	m_fGroundHeight = vPlayerPos.y;

	if (m_bJump == false)
		m_pTransformCom->Set_State(CTransform::STATE_TRANS, XMLoadFloat4(&vPlayerPos));
}

CSigrid * CSigrid::Create(DEVICE pDevice, DEVICE_CONTEXT pContext)
{
	CSigrid*		pInstance = new CSigrid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSigrid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSigrid::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CSigrid*		pInstance = new CSigrid(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CSigrid");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSigrid::Free()
{
	__super::Free();

	Safe_Release(m_pSigridState);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pSphereCol);
	Safe_Release(m_pOBBCol);
	Safe_Release(m_pNetSphereCol);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStateMachineCom);
}
