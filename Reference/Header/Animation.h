#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT						Save_Animation(HANDLE& hFile, DWORD& dwByte);
	HRESULT						Load_Animation(HANDLE& hFile, DWORD& dwByte);
	class CChannel*				Get_Channel(const string & strBoneName);
	void							Reset_Animation();

public:
	HRESULT						Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void							Update_Bones(_double dTimeDelta);
	_bool							Update_Lerp(_double dTimeDelta, CAnimation* pLastAnimation);

private:
	class CModel*					m_pModel = nullptr;

	_bool							m_bIsLoop = true;
	_bool							m_bIsFinish = false;
	_bool							m_bIsLerpFinish = false;
	string							m_strName = "";
	_double						m_dDuration = 0.0;
	_double						m_dTickPerSecond = 0.0;
	_double						m_dPlayTime = 0.0;
	_uint							m_iNumChannels = 0;
	vector<class CChannel*>		m_vecChannel;

public:
	static CAnimation*			Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void					Free() override;
};

END