#include "..\Public\CustomGameObject.h"
#include "GameInstance.h"
#include "GameUtility.h"

CCustomGameObject::CCustomGameObject(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CCustomGameObject::CCustomGameObject(const CCustomGameObject& rhs)
	: CGameObject(rhs)
	, m_iRendererComLevel(rhs.m_iRendererComLevel)
	, m_iVIBufferComLevel(rhs.m_iVIBufferComLevel)
	, m_iShaderComLevel(rhs.m_iShaderComLevel)
	, m_iTransformComLevel(rhs.m_iTransformComLevel)
	, m_iTextureComLevel(rhs.m_iTextureComLevel)
	, m_iModelComLevel(rhs.m_iModelComLevel)
	, m_iColliderComLevel(rhs.m_iColliderComLevel)
	, m_wstrRendererComTag(rhs.m_wstrRendererComTag)
	, m_wstrVIBufferComTag(rhs.m_wstrVIBufferComTag)
	, m_wstrShaderComTag(rhs.m_wstrShaderComTag)
	, m_wstrProtoTransformComTag(rhs.m_wstrProtoTransformComTag)
	, m_iNumTextureCom(rhs.m_iNumTextureCom)
	, m_wstrTextureComTag(rhs.m_wstrTextureComTag)
	, m_wstrModelComTag(rhs.m_wstrModelComTag)
	, m_wstrColliderComTag(rhs.m_wstrColliderComTag)
{
}

vector<pair<_uint, wstring>> CCustomGameObject::Get_PrototypeSaveData()
{
	vector<pair<_uint, wstring>>		vecPrototypeInfo;

	if (m_iRendererComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iRendererComLevel, m_wstrRendererComTag));
	if (m_iVIBufferComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iVIBufferComLevel, m_wstrVIBufferComTag));
	if (m_iShaderComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iShaderComLevel, m_wstrShaderComTag));
	if (m_iTransformComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iTransformComLevel, m_wstrProtoTransformComTag));
	if (m_iTextureComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iTextureComLevel, *m_wstrTextureComTag));
	if (m_iModelComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iModelComLevel, m_wstrModelComTag));
	if (m_iColliderComLevel != 1000)
		vecPrototypeInfo.push_back(pair<_uint, wstring>(m_iColliderComLevel, m_wstrColliderComTag));

	return vecPrototypeInfo;
}

HRESULT CCustomGameObject::Initialize_Prototype(const vector<pair<_uint, wstring>>& vecPrototypeInfo, _uint iNumTextureCom)
{
	m_iNumTextureCom = iNumTextureCom;

	if (m_iNumTextureCom != 0)
		m_wstrTextureComTag = new wstring[m_iNumTextureCom];
	_uint	iTextureComIndex = 0;

	COMPONENTTYPE	eType = COMPONENTTYPE_END;
	for (_uint i = 0; i < vecPrototypeInfo.size(); ++i)
	{
		eType = CGameUtility::CheckComponentTypeFromTag(vecPrototypeInfo[i].second);

		if (eType == COMPONENTTYPE_END)
			continue;

		else if (eType == COM_RENDERER)
		{
			m_iRendererComLevel = vecPrototypeInfo[i].first;
			m_wstrRendererComTag = vecPrototypeInfo[i].second;
		}
		else if (eType == COM_VIBUFFER)
		{
			m_iVIBufferComLevel = vecPrototypeInfo[i].first;
			m_wstrVIBufferComTag = vecPrototypeInfo[i].second;
		}
		else if (eType == COM_SHADER)
		{
			m_iShaderComLevel = vecPrototypeInfo[i].first;
			m_wstrShaderComTag = vecPrototypeInfo[i].second;
		}
		else if (eType == COM_TRANSFORM)
		{
			m_iTransformComLevel = vecPrototypeInfo[i].first;
			m_wstrProtoTransformComTag = vecPrototypeInfo[i].second;
		}
		else if (eType == COM_TEXTURE)
		{
			m_iTextureComLevel = vecPrototypeInfo[i].first;
			m_wstrTextureComTag[iTextureComIndex].resize(vecPrototypeInfo[i].second.size() + 1);
			m_wstrTextureComTag[iTextureComIndex++] = vecPrototypeInfo[i].second;
		}
		else if (eType == COM_MODEL)
		{
			m_iModelComLevel = vecPrototypeInfo[i].first;
			m_bHasModel = true;
			m_wstrModelComTag = vecPrototypeInfo[i].second;
		}
		else if (eType == COM_COLLIDER)
		{
			m_iColliderComLevel = vecPrototypeInfo[i].first;
			m_wstrColliderComTag = vecPrototypeInfo[i].second;
		}
	}

	return S_OK;
}

HRESULT CCustomGameObject::Initialize(const wstring& wstrPrototypeTag, void * pArg)
{
	GAMEOBJECTDESC	GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	if (nullptr != pArg)
		GameObjectDesc = *(GAMEOBJECTDESC*)pArg;

	if(m_wstrProtoTransformComTag != L"")
		FAILED_CHECK_RETURN(Add_Component(m_iTransformComLevel, m_wstrProtoTransformComTag, m_wstrTransformComTag, (CComponent**)&m_pTransformCom, this, &GameObjectDesc.TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CCustomGameObject::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_pModelCom != nullptr)
		m_pModelCom->Play_Animation(dTimeDelta);

	if (m_pColliderCom != nullptr)
		m_pColliderCom->Update(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
}

void CCustomGameObject::Late_Tick(_double dTimeDelta)
{
	__super::Late_Tick(dTimeDelta);

	if (m_pRendererCom != nullptr)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CCustomGameObject::Render()
{
	FAILED_CHECK_RETURN(__super::Render(), E_FAIL);
	FAILED_CHECK_RETURN(SetUp_ShaderResource(), E_FAIL);

	if (m_pModelCom != nullptr && m_pShaderCom != nullptr)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, L"g_DiffuseTexture");
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, L"g_NormalTexture");

			m_pModelCom->Render(m_pShaderCom, i, L"g_matBones");
		}
	}

	if (m_pVIBufferCom != nullptr && m_pShaderCom != nullptr)
	{
		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();
	}

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CCustomGameObject::SetUp_Component()
{
	if (m_wstrRendererComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(m_iRendererComLevel, m_wstrRendererComTag, L"Com_Renderer", (CComponent**)&m_pRendererCom, this), E_FAIL);
	if (m_wstrVIBufferComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(m_iVIBufferComLevel, m_wstrVIBufferComTag, L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom, this), E_FAIL);
	if (m_wstrShaderComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(m_iShaderComLevel, m_wstrShaderComTag, L"Com_Shader", (CComponent**)&m_pShaderCom, this), E_FAIL);
	if (m_wstrTextureComTag != nullptr)
	{
		for (_uint i = 0; i < m_iNumTextureCom; ++i)
		{
			_tchar		wszCloneTextureComTag[MAX_PATH] = L"";
			wsprintf(wszCloneTextureComTag, L"Com_Texture%d", i);
			FAILED_CHECK_RETURN(__super::Add_Component(m_iTextureComLevel, m_wstrTextureComTag[i], wstring(wszCloneTextureComTag), (CComponent**)&m_pTextureCom[i], this), E_FAIL);
		}
	}
	if (m_wstrModelComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(m_iModelComLevel, m_wstrModelComTag, L"Com_Model", (CComponent**)&m_pModelCom, this), E_FAIL);
	
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (m_wstrColliderComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(m_iColliderComLevel, m_wstrColliderComTag, L"Com_Collider", (CComponent**)&m_pColliderCom, this, &ColliderDesc), E_FAIL);

	return S_OK;
}

HRESULT CCustomGameObject::SetUp_ShaderResource()
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

CCustomGameObject * CCustomGameObject::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const vector<pair<_uint, wstring>>& vecPrototypeInfo, _uint iNumTextureCom)
{
	CCustomGameObject*	pInstance = new CCustomGameObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(vecPrototypeInfo, iNumTextureCom)))
	{
		MSG_BOX("Failed to Create : CCustomGameObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCustomGameObject::Clone(const wstring & wstrPrototypeTag, void * pArg)
{
	CCustomGameObject*	pInstance = new CCustomGameObject(*this);

	if (FAILED(pInstance->Initialize(wstrPrototypeTag, pArg)))
	{
		MSG_BOX("Failed to Clone : CCustomGameObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomGameObject::Free()
{
	__super::Free();

	if (!m_bIsClone && m_iNumTextureCom != 0)
	{
		Safe_Delete_Array(m_wstrTextureComTag);
	}
	if (m_bIsClone)
	{
		if (m_pRendererCom != nullptr)
			Safe_Release(m_pRendererCom);
		if (m_pVIBufferCom != nullptr)
			Safe_Release(m_pVIBufferCom);
		if (m_pShaderCom != nullptr)
			Safe_Release(m_pShaderCom);
		if (m_pTextureCom != nullptr)
		{
			for (_uint i = 0; i < m_iNumTextureCom; ++i)
				Safe_Release(m_pTextureCom[i]);
		}
		if (m_pModelCom != nullptr)
			Safe_Release(m_pModelCom);
		if (m_pColliderCom != nullptr)
			Safe_Release(m_pColliderCom);
	}
}