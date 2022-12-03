#include "..\Public\CustomGameObject.h"
#include "GameInstance.h"
#include "GameUtility.h"

CCustomGameObject::CCustomGameObject(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: CGameObject(pDevice, pContext)
{
}

CCustomGameObject::CCustomGameObject(const CCustomGameObject& rhs)
	: CGameObject(rhs)
	, m_wstrRendererComTag(rhs.m_wstrRendererComTag)
	, m_wstrVIBufferComTag(rhs.m_wstrVIBufferComTag)
	, m_wstrShaderComTag(rhs.m_wstrShaderComTag)
	, m_wstrProtoTransformComTag(rhs.m_wstrProtoTransformComTag)
	, m_wstrTextureComTag(rhs.m_wstrTextureComTag)
	, m_wstrModelComTag(rhs.m_wstrModelComTag)
	, m_iNumTextureCom(rhs.m_iNumTextureCom)
{
}

HRESULT CCustomGameObject::Initialize_Prototype(const vector<wstring>& vecPrototypeTags, _uint iNumTextureCom)
{
	m_iNumTextureCom = iNumTextureCom;

	if (m_iNumTextureCom != 0)
		m_wstrTextureComTag = new wstring[m_iNumTextureCom];
	_uint	iTextureComIndex = 0;

	COMPONENTTYPE	eType = COMPONENTTYPE_END;
	for (_uint i = 0; i < vecPrototypeTags.size(); ++i)
	{
		eType = CGameUtility::CheckComponentTypeFromTag(vecPrototypeTags[i]);

		if (eType == COMPONENTTYPE_END)
			continue;

		else if (eType == COM_RENDERER)
			m_wstrRendererComTag = vecPrototypeTags[i];

		else if (eType == COM_VIBUFFER)
			m_wstrVIBufferComTag = vecPrototypeTags[i];

		else if (eType == COM_SHADER)
			m_wstrShaderComTag = vecPrototypeTags[i];

		else if (eType == COM_TRANSFORM)
			m_wstrProtoTransformComTag = vecPrototypeTags[i];

		else if (eType == COM_TEXTURE)
		{
			m_wstrTextureComTag[iTextureComIndex].resize(vecPrototypeTags[i].size() + 1);
			m_wstrTextureComTag[iTextureComIndex++] = vecPrototypeTags[i];
		}

		else if (eType == COM_MODEL)
		{
			m_bHasModel = true;
			m_wstrModelComTag = vecPrototypeTags[i];
		}
	}

	return S_OK;
}

HRESULT CCustomGameObject::Initialize(void * pArg)
{
	GAMEOBJECTDESC	GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	if (nullptr != pArg)
		GameObjectDesc = *(GAMEOBJECTDESC*)pArg;

	if(m_wstrProtoTransformComTag != L"")
		FAILED_CHECK_RETURN(Add_Component(CGameInstance::Get_StaticLevelIndex(), m_wstrProtoTransformComTag, m_wstrTransformComTag, (CComponent**)&m_pTransformCom, &GameObjectDesc.TransformDesc), E_FAIL);

	FAILED_CHECK_RETURN(SetUp_Component(), E_FAIL);

	return S_OK;
}

void CCustomGameObject::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
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

	return S_OK;
}

HRESULT CCustomGameObject::SetUp_Component()
{
	if (m_wstrRendererComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), m_wstrRendererComTag, L"Com_Renderer", (CComponent**)&m_pRendererCom), E_FAIL);
	if (m_wstrVIBufferComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), m_wstrVIBufferComTag, L"Com_VIBuffer", (CComponent**)&m_pVIBufferCom), E_FAIL);
	if (m_wstrShaderComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, m_wstrShaderComTag, L"Com_Shader", (CComponent**)&m_pShaderCom), E_FAIL);
	if (m_wstrTextureComTag != nullptr)
	{

	}
	if (m_wstrModelComTag != L"")
		FAILED_CHECK_RETURN(__super::Add_Component(LEVEL_TESTSTAGE, m_wstrModelComTag, L"Com_Model", (CComponent**)&m_pModelCom), E_FAIL);

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

CCustomGameObject * CCustomGameObject::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const vector<wstring>& vecPrototypeTags, _uint iNumTextureCom)
{
	CCustomGameObject*	pInstance = new CCustomGameObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(vecPrototypeTags, iNumTextureCom)))
	{
		MSG_BOX("Failed to Create : CCustomGameObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCustomGameObject::Clone(void * pArg)
{
	CCustomGameObject*	pInstance = new CCustomGameObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
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
	}
}