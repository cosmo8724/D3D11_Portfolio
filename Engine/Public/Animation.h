#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT					Initialize(aiAnimation* pAIAnimation);

private:
	string						m_strName = "";
	_double					m_dDuration = 0.0;
	_double					m_TickPerSecond = 0.0;

	_uint						m_iNumAnimationBone = 0;
	vector<class CBone*>	m_vecAnimationBone;

public:
	static CAnimation*		Create(aiAnimation* pAIAnimation);
	virtual void				Free() override;
};

END