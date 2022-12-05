#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel * pModel)
{
	m_strName = pAIAnimation->mName.data;
	m_dDuration = pAIAnimation->mDuration;
	m_dTickPerSecond = pAIAnimation->mTicksPerSecond;
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*	pAINodeAnim = pAIAnimation->mChannels[i];

		CChannel*		pChannel = CChannel::Create(pAINodeAnim, pModel);
		NULL_CHECK_RETURN(pChannel, E_FAIL);

		m_vecChannel.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_Bones(_double dTimeDelta)
{
	if (!m_bIsLoop && m_bIsFinish)
		return;

	m_dPlayTime += m_dTickPerSecond * dTimeDelta;

	if (m_dPlayTime >= m_dDuration)
	{
		m_dPlayTime = 0.0;
		m_bIsFinish = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (m_bIsFinish == true)
			m_vecChannel[i]->Reset_KeyFrameIndex();

		m_vecChannel[i]->Update_matTransform(m_dPlayTime);
	}

	if (m_bIsFinish)
		m_bIsFinish = false;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel * pModel)
{
	CAnimation*		pInstance = new CAnimation;

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_vecChannel)
		Safe_Release(pChannel);
	m_vecChannel.clear();
}