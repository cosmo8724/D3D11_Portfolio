#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANS, STATE_END };

	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	} TRANSFORMDESC;

private:
	CTransform(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	const _vector&	Get_State(STATE eState) const { return XMLoadFloat4x4(&m_matWorld).r[eState]; }
	void				Set_State(STATE eState, _fvector _vState) {
		_float4	vState;
		XMStoreFloat4(&vState, _vState);
		memcpy(&m_matWorld.m[eState], &vState, sizeof(vState));
	}

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;

public:
	void				Go_Straight(_double dTimeDelta);
	void				Go_BackWard(_double dTimeDelta);
	void				Go_Left(_double dTimeDelta);
	void				Go_Right(_double dTimeDelta);

	void				Turn(_fvector vAxis, _double dTimeDelta);
	void				Rotation(_fvector vAxis, _float fRadian);

private:
	_float4x4				m_matWorld;
	TRANSFORMDESC	m_TransformDesc;

public:
	static CTransform*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END