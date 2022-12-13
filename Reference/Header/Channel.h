#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT				Save_Channel(HANDLE& hFile, DWORD& dwByte);
	HRESULT				Load_Channel(HANDLE& hFile, DWORD& dwByte);
	const string&			Get_ChannelName() const { return m_strName; }

public:
	HRESULT				Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void					Update_matTransform(_double dPlayTime);
	_bool					Update_Lerp(_double dPlayTime, CChannel* pLastChannel);
	void					Reset_KeyFrameIndex() { m_iCurKeyFrameIndex = 0; }
	void					Reset_LerpFrameIndex() { m_iLerpFrameIndex = -1; }

private:
	class CModel*			m_pModel = nullptr;

	string					m_strName = "";
	class CBone*			m_pBone = nullptr;

	_uint					m_iNumKeyFrames = 0;
	vector<KEYFRAME>	m_vecKeyFrame;
	typedef vector<KEYFRAME>		KEYFRAMES;

	_uint					m_iCurKeyFrameIndex = 0;

	_bool					m_bLerpFinished = false;
	_double				m_dLerpRatio = 0.0;
	_int					m_iLerpFrameIndex = -1;

	_vector				m_vLastScale;
	_vector				m_vLastRotation;
	_vector				m_vLastPosition;
	_matrix				m_vLastmatTransform;

public:
	static CChannel*		Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void			Free() override;
};

END