#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"

#ifdef _DEBUG
#define new DBG_NEW 
#endif

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Save_Animation(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = (_uint)m_strName.length() + 1;
	const char*	pName = m_strName.c_str();
	WriteFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	WriteFile(hFile, &m_dDuration, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_dTickPerSecond, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_bIsLoop, sizeof(_bool), &dwByte, nullptr);

	WriteFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	for (auto& pChannel : m_vecChannel)
	{
		if (FAILED(pChannel->Save_Channel(hFile, dwByte)))
		{
			MSG_BOX("Failed to Save : Channel");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CAnimation::Load_Animation(HANDLE & hFile, DWORD & dwByte)
{
	_uint			iNameLength = 0;
	ReadFile(hFile, &iNameLength, sizeof(_uint), &dwByte, nullptr);

	char*			pName = new char[iNameLength];
	ReadFile(hFile, pName, sizeof(char) * iNameLength, &dwByte, nullptr);

	m_strName = pName;

	Safe_Delete_Array(pName);

	ReadFile(hFile, &m_dDuration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_dTickPerSecond, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_bIsLoop, sizeof(_bool), &dwByte, nullptr);

	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	m_vecChannel.reserve(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(nullptr, m_pModel);
		NULL_CHECK_RETURN(pChannel, E_FAIL);
		FAILED_CHECK_RETURN(pChannel->Load_Channel(hFile, dwByte), E_FAIL);

		m_vecChannel.push_back(pChannel);
	}

	return S_OK;
}

CChannel * CAnimation::Get_Channel(const string & strBoneName)
{
	auto	iter = find_if(m_vecChannel.begin(), m_vecChannel.end(), [&](CChannel* pChannel)->bool {
		return strBoneName == pChannel->Get_ChannelName();
	});

	if (iter == m_vecChannel.end())
		return nullptr;

	return *iter;
}

void CAnimation::Reset_Animation()
{
	for (auto& pChannel : m_vecChannel)
		pChannel->Reset_KeyFrameIndex();
	m_dPlayTime = 0.0;
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel * pModel)
{
	m_pModel = pModel;

	if (pAIAnimation == nullptr)
		return S_OK;

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

_bool CAnimation::Update_Lerp(_double dTimeDelta, CAnimation * pLastAnimation)
{
	m_bIsLerpFinish = false;

	m_dPlayTime += m_dTickPerSecond * dTimeDelta;

	for (auto& CurChannel : m_vecChannel)
	{
		CChannel*		pLastChannel = pLastAnimation->Get_Channel(CurChannel->Get_ChannelName());

		if (pLastChannel != nullptr)
		{
			m_bIsLerpFinish = CurChannel->Update_Lerp(m_dPlayTime, pLastChannel);
		}
		else
			CurChannel->Update_matTransform(dTimeDelta);
	}

	return m_bIsLerpFinish;

	//if (m_bIsLerpFinish)
	//{
	//	m_dPlayTime = 0.0;

	//	for (auto Channel : m_vecChannel)
	//	{
	//		Channel->Reset_KeyFrameIndex();
	//		Channel->Reset_LerpFrameIndex();
	//	}

	//	m_bIsLerpFinish = false;

	//	return false;
	//}
	//else
	//{
	//	for (auto& CurChannel : m_vecChannel)
	//	{
	//		CChannel*		pLastChannel = pLastAnimation->Get_Channel(CurChannel->Get_ChannelName());

	//		if (pLastChannel != nullptr)
	//			m_bIsLerpFinish = CurChannel->Update_Lerp(m_dPlayTime, pLastChannel, bIsFinish);
	//	}
	//	return true;
	//}
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