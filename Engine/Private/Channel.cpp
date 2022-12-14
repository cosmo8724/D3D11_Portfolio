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

void CChannel::Update_matTransform(_double dPlayTime)
{
	_vector	vScale;
	_vector	vRotation;
	_vector	vPosition;
	_matrix	matTransform;

	if (dPlayTime >= m_vecKeyFrame.back().dTime)
	{
		m_vLastScale = vScale = XMLoadFloat3(&m_vecKeyFrame.back().vScale);
		m_vLastRotation = vRotation = XMLoadFloat4(&m_vecKeyFrame.back().vRotation);
		vPosition = XMLoadFloat3(&m_vecKeyFrame.back().vPosition);
		m_vLastPosition = vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (dPlayTime >= m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime)
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

_bool CChannel::Update_Lerp(_double dPlayTime, CChannel * pLastChannel)
{
	_vector	vScale, vRotation, vPosition;
	_matrix	matTransform;

	/*if (m_bLerpFinished)
		return true;

	if (dPlayTime >= m_vecKeyFrame.back().dTime)
	{
		m_bLerpFinished = true;
		vScale = XMLoadFloat3(&m_vecKeyFrame.back().vScale);
		vRotation = XMLoadFloat4(&m_vecKeyFrame.back().vRotation);
		vPosition = XMLoadFloat3(&m_vecKeyFrame.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
		return true;
	}*/

	/*if (dPlayTime >= m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime)
	{
		m_bLerpFinished = true;
		++m_iCurKeyFrameIndex;
		return true;
	}*/

	_double	dRatio = m_dLerpRatio;//(dPlayTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime) / (m_vecKeyFrame[m_iCurKeyFrameIndex + 1].dTime - m_vecKeyFrame[m_iCurKeyFrameIndex].dTime);

	m_dLerpRatio += 0.03;

	if (m_dLerpRatio >= 1.0)
	{
		m_dLerpRatio = 0.0;
		return true;
	}
	vScale = XMVectorLerp(XMLoadFloat3(&pLastChannel->m_vecKeyFrame[pLastChannel->m_iCurKeyFrameIndex].vScale), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vScale), (_float)dRatio);
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&pLastChannel->m_vecKeyFrame[pLastChannel->m_iCurKeyFrameIndex].vRotation), XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vRotation), (_float)dRatio);
	vPosition = XMVectorLerp(XMLoadFloat3(&pLastChannel->m_vecKeyFrame[pLastChannel->m_iCurKeyFrameIndex].vPosition), XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex + 1].vPosition), (_float)dRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	vScale = XMVectorLerp(vScale, XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vScale), (_float)dRatio);
	vRotation = XMQuaternionSlerp(vRotation, XMLoadFloat4(&m_vecKeyFrame[m_iCurKeyFrameIndex].vRotation), (_float)dRatio);
	vPosition = XMVectorLerp(vPosition, XMLoadFloat3(&m_vecKeyFrame[m_iCurKeyFrameIndex].vPosition), (_float)dRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	matTransform = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_matTransform(matTransform);

	return false;

	/*_vector	vScale, vRotation, vPosition;
	_matrix	matTransform;

	if (m_iLerpFrameIndex == -1)
	{
		while (dPlayTime >= m_vecKeyFrame[m_iLerpFrameIndex + 1].dTime)
		{
			m_iLerpFrameIndex++;

			if ((_int)m_iNumKeyFrames <= m_iLerpFrameIndex + 1)
			{
				m_iLerpFrameIndex -= 1;
				break;
			}
		}
	}

	_uint	iSour = 0, iDest = 0;

	iSour = m_iLerpFrameIndex;

	_uint	iLastNumKeyframes = (_uint)pLastChannel->m_vecKeyFrame.size();
	if (iLastNumKeyframes <= iSour)
		iSour = iLastNumKeyframes - 1;

	m_dLerpRatio += 0.1;

	_vector	vLastScale, vCurScale;
	_vector	vLastRotation, vCurRotation;
	_vector	vLastPosition, vCurPosition;

	vLastScale = XMLoadFloat3(&pLastChannel->m_vecKeyFrame[iSour].vScale);
	vCurScale = XMLoadFloat3(&pCurrentChannel->m_vecKeyFrame[iDest].vScale);

	vLastRotation = XMLoadFloat4(&pLastChannel->m_vecKeyFrame[iSour].vRotation);
	vCurRotation = XMLoadFloat4(&pCurrentChannel->m_vecKeyFrame[iDest].vRotation);

	vLastPosition = XMLoadFloat3(&pLastChannel->m_vecKeyFrame[iSour].vPosition);
	vCurPosition = XMLoadFloat3(&pCurrentChannel->m_vecKeyFrame[iDest].vPosition);

	vScale = XMVectorLerp(vLastScale, vCurScale, (_float)m_dLerpRatio);
	vRotation = XMQuaternionSlerp(vLastRotation, vCurRotation, (_float)m_dLerpRatio);
	vPosition = XMVectorLerp(vLastPosition, vCurPosition, (_float)m_dLerpRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	matTransform = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_matTransform(matTransform);

	if (m_dLerpRatio >= 1.0)
	{
		m_dLerpRatio = 0.0;
		bIsFinish = true;
		return true;
	}

	return false;*/
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