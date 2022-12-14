#include "stdafx.h"
#include "..\Public\Bone.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

CBone::CBone()
{
}

HRESULT CBone::Save_Bone(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = (_uint)m_strName.length() + 1;
	const char*	pName = m_strName.c_str();
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_matOffset, sizeof(_float4x4), &dwByte, nullptr);
	WriteFile(hFile, &m_matTransform, sizeof(_float4x4), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumChild, sizeof(_uint), &dwByte, nullptr);

	return S_OK;
}

HRESULT CBone::Save_BoneName(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = (_uint)m_strName.length() + 1;
	const char*	pName = m_strName.c_str();
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	return S_OK;
}

HRESULT CBone::Load_Bone(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	
	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	m_strName = pName;

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_matOffset, sizeof(_float4x4), &dwByte, nullptr);
	ReadFile(hFile, &m_matTransform, sizeof(_float4x4), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumChild, sizeof(_uint), &dwByte, nullptr);

	return S_OK;
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone * pParent, _uint iNumChild)
{
	if (pParent != nullptr)
	{
		m_pParent = pParent;
		Safe_AddRef(m_pParent);
	}

	if (pAINode == nullptr)
		return S_OK;

	m_strName = pAINode->mName.data;


	m_iNumChild = iNumChild;

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

CBone * CBone::Create(aiNode * pAINode, CBone * pParent, _uint iNumChild)
{
	CBone*	pInstance = new CBone;

	if (FAILED(pInstance->Initialize(pAINode, pParent, iNumChild)))
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