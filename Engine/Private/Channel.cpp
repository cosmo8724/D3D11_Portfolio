#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, CModel * pModel)
{
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

void CChannel::Update_matTransform(_double dPlayTime)
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
		if (dPlayTime >= m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime)
			++m_iCurKeyFrameIndex;

		_double	dRatio = (dPlayTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime)
			/ (m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime);

		vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vScale), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vScale), (_float)dRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex].vRotation), XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vRotation), (_float)dRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vPosition), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vPosition), (_float)dRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);
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