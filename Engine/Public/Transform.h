#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANS, STATE_END };
	enum DIRECTION { DIR_W, DIR_A, DIR_S, DIR_D, DIR_WA, DIR_WD, DIR_SA, DIR_SD, DIR_LOOK, DIR_END };

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
	TRANSFORMDESC&	Get_TransformDesc() { return m_TransformDesc; }
	const _float4x4&Get_WorldMatrix() const { return m_matWorld; }
	const _matrix	Get_WorldMatrix_Inverse() { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matWorld)); }
	const _vector&	Get_State(STATE eState) const { return XMLoadFloat4x4(&m_matWorld).r[eState]; }
	const _float3		Get_Scale() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))),
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}
	_double&			Get_Speed() { return m_TransformDesc.dSpeedPerSec; }
	void				Set_WorldMatrix(_float4x4 matWorld) { m_matWorld = matWorld; }
	void				Set_Scale(STATE eState, _float fScale);
	void				Set_Scale(_float3 vScale);
	void				Scaling(STATE eState, _float fScale);
	void				Set_State(STATE eState, _fvector _vState) {
		_float4	vState;
		XMStoreFloat4(&vState, _vState);
		memcpy(&m_matWorld.m[eState], &vState, sizeof(vState));
	}
	void				Set_Speed(_double dSpeedPerSec) { m_TransformDesc.dSpeedPerSec = dSpeedPerSec; }

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual void		ImGui_RenderProperty() override;

	HRESULT			Bind_ShaderResource(class CShader * pShaderCom, const wstring wstrConstantName);

public:
	void				Go_Direction(_fvector vDirection, _double dTimeDelta);
	void				Go_Straight(_double dTimeDelta);
	void				Go_BackWard(_double dTimeDelta);
	void				Go_Left(_double dTimeDelta);
	void				Go_Right(_double dTimeDelta);
	void				Go_Up(_double dTimeDelta);
	void				Go_Down(_double dTimeDelta);
	void				Speed_Up(_bool bKeyState);

	void				Jump(_double dTimeDelta, _float& fGravity, _float& fCurJumpSpeed);
	void				Dash(_double dTimeDelta, _float& fFriction, _float& fCurDashTickCount, _fmatrix matCamWorld, DIRECTION eDir);

	void				Turn(_fvector vAxis, _double dTimeDelta);
	void				Rotation(_fvector vAxis, _float fRadian);
	void				RotationFromNow(_fvector vAxis, _float fRadian);
	void				Orbit(_fvector vTargetPos, _fvector vAxis, _float fDistance, _double dTimeDelta);

	void				LookAt(_fvector vTargetPos);
	void				Chase(_fvector vTargetPos, _double dTimeDelta, _float fLimit = 0.1f);

private:
	_float4x4				m_matWorld;
	TRANSFORMDESC	m_TransformDesc;
	_double				m_dInitSpeed = 0.0;

public:
	static CTransform*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END