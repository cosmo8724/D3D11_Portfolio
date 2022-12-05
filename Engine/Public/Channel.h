#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT				Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void					Update_matTransform(_double dPlayTime);
	void					Reset_KeyFrameIndex() { m_iCurKeyFrameIndex = 0; }

private:
	string					m_strName = "";
	class CBone*			m_pBone = nullptr;

	_uint					m_iNumKeyFrames = 0;
	vector<KEYFRAME>	m_vecKeyFrame;
	typedef vector<KEYFRAME>		KEYFRAMES;

	_uint					m_iCurKeyFrameIndex = 0;

public:
	static CChannel*		Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void			Free() override;
};

END