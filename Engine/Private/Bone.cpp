#include "..\Public\Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone * pParent)
{
	m_strName = pAINode->mName.data;

	if (pParent != nullptr)
	{
		m_pParent = pParent;
		Safe_AddRef(m_pParent);
	}

	XMStoreFloat4x4(&m_matOffset, XMMatrixIdentity());

	memcpy(&m_matTransform, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_matTransform, XMMatrixTranspose(XMLoadFloat4x4(&m_matTransform)));

	XMStoreFloat4x4(&m_matCombindTransform, XMMatrixIdentity());

	return S_OK;
}

void CBone::Compute_CombindTransformMatrix()
{
	if (m_pParent == nullptr)
		m_matCombindTransform = m_matTransform;
	else
		XMStoreFloat4x4(&m_matCombindTransform, XMLoadFloat4x4(&m_matTransform) * XMLoadFloat4x4(&m_pParent->m_matCombindTransform));
}

CBone * CBone::Create(aiNode * pAINode, CBone * pParent)
{
	CBone*	pInstance = new CBone;

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
	{
		MSG_BOX("Failed to Create : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
	if (m_pParent != nullptr)
		Safe_Release(m_pParent);
}