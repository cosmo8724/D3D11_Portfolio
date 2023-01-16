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
	string&						Get_AnimationName() { return m_strName; }
	class CChannel*				Get_Channel(const string & strBoneName);
	_double&						Get_AnimationTickPerSecond() { return m_dTickPerSecond; }
	_bool							Get_AnimationFinish() { return m_bIsFinish; }
	_bool&							Get_AnimationLoop() { return m_bIsLoop; }
	_float							Get_AnimationProgress() { return _float(m_dPlayTime / m_dDuration); }
	void							Set_AnimationTickPerSecond(_double dTickPerSecond) { m_dTickPerSecond = dTickPerSecond; }
	void							Reset_Animation();

public:
	HRESULT						Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void							Update_Bones(_double dTimeDelta, const wstring& wstrRootBoneName);
	void							Update_Lerp(_double dTimeDelta, _float fRatio, const wstring& wstrRootBoneName);

private:
	class CModel*					m_pModel = nullptr;

	_bool							m_bIsLoop = false;
	_bool							m_bIsFinish = false;
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