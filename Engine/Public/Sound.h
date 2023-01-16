#pragma once
#include "Base.h"

#include <io.h>
#include "Fmod/fmod.h"
#include "Fmod/fmod.hpp"
// #pragma comment(lib, "fmodL_vc.lib")

BEGIN(Engine)

class CSound : public CBase
{
public:			
	enum VOLUME_TYPE { VOLUME_DEFAULT, VOLUME_3D_RANGE, VOLUME_USE, VOLUME_END };

	typedef struct ENGINE_DLL tag3DSoundDesc
	{
		_bool					bIs3D; 
		_float					fRange;

		class CTransform*	pStartTransform;
		class CTransform*	pTargetTransform;
	} SOUND_DESC;
		
protected:
	CSound();
	virtual ~CSound() = default;

public:
	FMOD_SOUND*			GetSoundPtr() { return m_pSound; }
	const _float&				GetVolume(VOLUME_TYPE eType) const { return m_fVolume[eType]; }
	const _int&				Get_ChannelIndex() const { return m_iChannelIndex; }
	void						Set_SoundDesc(SOUND_DESC& SoundDesc);
	void						Set_Volume(FMOD_CHANNEL* pChannel, _float fVolume);
	static void				Set_MasterVolume(_float fVolume) { m_fMasterVolume = fVolume; }
	void						Set_ChannelIndex(_int iChannelIndex) { m_iChannelIndex = iChannelIndex; }

public:
	HRESULT					Initialize(FMOD_SOUND* pSound);
	void						Tick(_double TimeDelta, FMOD_CHANNEL* pChannel);

public:
	void						UpdateUseVolume(FMOD_CHANNEL* pChannel);

private:
	FMOD_SOUND*			m_pSound = nullptr;
	SOUND_DESC				m_SoundDesc;

	static _float				m_fMasterVolume;
	_float						m_fVolume[VOLUME_END] = { 0.5f, 1.f, 0.5f };
	_int						m_iChannelIndex = -1;

private:
	void						Clear_SoundDesc();

public:	
	static CSound*			Create(FMOD_SOUND* pSound);
	virtual void				Free() override;
};
END
