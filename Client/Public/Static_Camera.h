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
	void							Set_OwnerTransform(CTransform* pTransformCom) { m_pOwnerTransform = pTransformCom; }
	void							Init_Position();

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void					Tick(_double dTimeDelta) override;
	virtual void					Late_Tick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;
	
public:
	void							Pitch(_float dTimeDelta);
	void							Yaw(_float dTimeDelta);
	void							Roll(_float dTimeDelta);

private:
	CTransform*					m_pOwnerTransform = nullptr;
	_float							m_fDistanceFromTarget = 5.f;

	_bool							m_bMouseFix = true;

	_long							m_MouseMoveX = 0;
	_long							m_MouseMoveY = 0;

private:
	HRESULT						SetUp_Component();

public:
	static CStatic_Camera*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*		Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void					Free() override;
};

END