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
	_vector						Get_CameraLookAt() { return m_vDir; }
	void							Set_CameraDesc(CTransform* pTransformCom, CBone* pSocket, _fmatrix matPivot) {
		m_pOwnerTransform = pTransformCom;
		m_pSocket = pSocket;
		m_matSocketPivot = matPivot;
	}

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
	CBone*						m_pSocket = nullptr;
	_matrix						m_matSocketPivot;

	_float4x4						m_matCamera;
	_vector						m_vDir;

private:
	HRESULT						SetUp_Component();

public:
	static CStatic_Camera*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*		Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void					Free() override;
};

END