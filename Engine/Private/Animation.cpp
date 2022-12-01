#include "..\Public\Animation.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation)
{
	return S_OK;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation)
{
	CAnimation*		pInstance = new CAnimation;

	if (FAILED(pInstance->Initialize(pAIAnimation)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
}