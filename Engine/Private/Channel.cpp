#include "stdafx.h"
#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

CChannel::CChannel()
{
}

HRESULT CChannel::Save_Channel(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = (_uint)m_strName.length() + 1;
	const char*	pName = m_strName.c_str();
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
	
	for (auto& tKeyFrame : m_vecKeyFrame)
		WriteFile(hFile, &tKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);

	return S_OK;
}

HRESULT CChannel::Load_Channel(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);

	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	m_strName = pName;

	m_pBone = m_pModel->Get_BoneFromEntireBone(m_strName);
	Safe_AddRef(m_pBone);

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
	m_vecKeyFrame.reserve(m_iNumKeyFrames);

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME		tKeyFrame;
		ZeroMemory(&tKeyFrame, sizeof(KEYFRAME));

		ReadFile(hFile, &tKeyFrame, sizeof(KEYFRAME), &dwByte, nullptr);

		m_vecKeyFrame.push_back(tKeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, CModel * pModel)
{
	m_pModel = pModel;

	if (pAIChannel == nullptr)
		return S_OK;

	m_strName = pAIChannel->mNodeName.data;

	m_pBone = pModel->Get_BoneFromEntireBone(m_strName);
	Safe_AddRef(m_pBone);

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	m_vecKeyFrame.reserve(m_iNumKeyFrames);

	_float3	vScale;
	_float4	vRotation;
	_float3	vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME		KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.dTime = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.dTime = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.dTime = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_vecKeyFrame.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_matTransform(_double dPlayTime, const wstring & wstrRootBoneName)
{
	_vector	vScale;
	_vector	vRotation;
	_vector	vPosition;
	_matrix	matTransform;

	if (dPlayTime >= m_vecKeyFrame.back().dTime)
	{
		vScale = XMLoadFloat3(&m_vecKeyFrame.back().vScale);
		vRotation = XMLoadFloat4(&m_vecKeyFrame.back().vRotation);
		vPosition = XMLoadFloat3(&m_vecKeyFrame.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (dPlayTime >= m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime)
			++m_iCurKeyFrameIndex;

		_float fRatio = _float((dPlayTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime)
			/ (m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime));

		vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vScale), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex].vRotation), XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vPosition), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		if (wstrRootBoneName != L"")
		{
			string	strRootBoneName = "";
			strRootBoneName.assign(wstrRootBoneName.begin(), wstrRootBoneName.end());

			if (m_pBone->Get_Name() == strRootBoneName)
			{
				vPosition = XMVectorSet(0.f, XMVectorGetY(vPosition), 0.f, 1.f);
				//vRotation = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			}
		}
	}

	matTransform = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_matTransform(matTransform);
}

void CChannel::Update_Lerp(_double dPlayTime, _float fRatio, const wstring & wstrRootBoneName)
{
	_vector	vBaseScale, vBaseRotation, vBasePosition;
	_vector	vScale, vRotation, vPosition;
	_matrix	matTransform = m_pBone->Get_matTransform();

	XMMatrixDecompose(&vBaseScale, &vBaseRotation, &vBasePosition, matTransform);

	if (dPlayTime >= m_vecKeyFrame.back().dTime)
	{
		vScale = XMLoadFloat3(&m_vecKeyFrame.back().vScale);
		vRotation = XMLoadFloat4(&m_vecKeyFrame.back().vRotation);
		vPosition = XMLoadFloat3(&m_vecKeyFrame.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (dPlayTime >= m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime)
			++m_iCurKeyFrameIndex;

		_float fTmp = _float((dPlayTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime)
			/ (m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime));

		vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vScale), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vScale), fTmp);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex].vRotation), XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vRotation), fTmp);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vPosition), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vPosition), fTmp);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	vScale = XMVectorLerp(vBaseScale, vScale, fRatio);
	vRotation = XMQuaternionSlerp(vBaseRotation, vRotation, fRatio);
	vPosition = XMVectorLerp(vBasePosition, vPosition, fRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	if (wstrRootBoneName != L"")
	{
		string	strRootBoneName = "";
		strRootBoneName.assign(wstrRootBoneName.begin(), wstrRootBoneName.end());

		if (m_pBone->Get_Name() == strRootBoneName)
		{
			vPosition = XMVectorSet(0.f, XMVectorGetY(vPosition), 0.f, 1.f);
			//vRotation = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
	}

	matTransform = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_matTransform(matTransform);
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, CModel* pModel)
{
	CChannel*		pInstance = new CChannel;

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);

	m_vecKeyFrame.clear();
}