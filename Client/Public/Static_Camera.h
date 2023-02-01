#pragma once
#include "Client_Define.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
class CBone;
END

BEGIN(Client)

class CStatic_Camera final : public CCamera
{
private:
	CStatic_Camera(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CStatic_Camera(const CStatic_Camera& rhs);
	virtual ~CStatic_Camera() = default;

public:
	const _long&					Get_MouseMoveX() const { return m_MouseMoveX; }
	_float&						Get_CloseAngle() { return m_fCloseAngle; }
	void							Set_OwnerTransform(CTransform* pTransformCom) { m_pOwnerTransform = pTransformCom; }
	void							Set_TimeScale(_double dTimeDelta) { m_dTimeScale = dTimeDelta; }
	void							Set_MouseFix(_bool bMouseFix) { m_bMouseFix = bMouseFix; }
	void							Init_Position();

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void					Tick(_double dTimeDelta) override;
	virtual void					Late_Tick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	CTransform*					m_pOwnerTransform = nullptr;
	_float							m_fDistanceFromTarget = 5.f;

	_double						m_dTimeScale = 1.0;
	_bool							m_bMouseFix = true;

	_long							m_MouseMoveX = 0;
	_long							m_MouseMoveY = 0;

	_bool							m_bShopOpen = false;
	_bool							m_bNeedLerp = false;
	_float							m_fCloseAngle = 25.f;

	_float4						m_vLerpStartEye;
	_float4						m_vLerpStartAt;
	_double						m_dLerpTime = 0.0;

private:
	HRESULT						SetUp_Component();

public:
	static CStatic_Camera*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*		Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void					Free() override;
};

END