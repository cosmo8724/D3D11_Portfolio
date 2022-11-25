#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANS, STATE_END };

	typedef struct tagTransformDesc
	{
		_double	dSpeedPerSec;
		_double	dRotationPerSec;
	} TRANSFORMDESC;

private:
	CTransform(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	const _matrix	Get_WorldMatrix_Inverse() { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matWorld)); }
	const _vector&	Get_State(STATE eState) const { return XMLoadFloat4x4(&m_matWorld).r[eState]; }
	const _float3		Get_Scale() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}
	void				Set_Scale(STATE eState, _float fScale);
	void				Set_Scale(_float3 vScale);
	void				Scaling(STATE eState, _float fScale);
	void				Set_State(STATE eState, _fvector _vState) {
		_float4	vState;
		XMStoreFloat4(&vState, _vState);
		memcpy(&m_matWorld.m[eState], &vState, sizeof(vState));
	}

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	virtual void		ImGui_RenderProperty() override;

	HRESULT			Bind_ShaderResource(class CShader * pShaderCom, const wstring wstrConstantName);

public:
	void				Go_Straight(_double dTimeDelta);
	void				Go_BackWard(_double dTimeDelta);
	void				Go_Left(_double dTimeDelta);
	void				Go_Right(_double dTimeDelta);
	void				Go_Up(_double dTimeDelta);
	void				Go_Down(_double dTimeDelta);
	void				Speed_Up(_bool bKeyState);

	void				Turn(_fvector vAxis, _double dTimeDelta);
	void				Rotation(_fvector vAxis, _float fRadian);

	void				LookAt(_fvector vTargetPos);
	void				Chase(_fvector vTargetPos, _double dTimeDelta, _float fLimit = 0.1f);

private:
	_float4x4				m_matWorld;
	TRANSFORMDESC	m_TransformDesc;
	_double				m_dInitSpeed = 0.0;

public:
	static CTransform*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END