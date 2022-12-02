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
	m_dPlayTime += m_dTickPerSecond * dTimeDelta;

	for (_uint i = 0; i < m_iNumChannels; ++i)
		m_vecChannel[i]->Update_matTransform(m_dPlayTime);
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