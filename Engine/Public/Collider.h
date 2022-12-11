#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum COLLIDERTYPE { COLLIDER_SPHERE, COLLIDER_AABB, COLLIDER_OBB, COLLIDERTYPE_END };

	typedef struct tagColliderDesc
	{
		_float3		vPosition;
		_float3		vSize;
		_float3		vRotation;
	} COLLIDERDESC;

private:
	CCollider(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	const COLLIDERTYPE&	Get_ColliderType() const { return m_eType; }

public:
	virtual HRESULT			Initialize_Prototype(COLLIDERTYPE eType);
	virtual HRESULT			Initialize(void* pArg) override;

public:
	void						Update(_fmatrix matTransform);

#ifdef _DEBUG
public:
	HRESULT					Render();
#endif

private:
	_bool						m_bIsCollide = false;
	COLLIDERTYPE			m_eType = COLLIDERTYPE_END;
	BoundingSphere*			m_pSphere_Original = nullptr;
	BoundingSphere*			m_pSphere = nullptr;
	BoundingBox*				m_pAABB_Original = nullptr;
	BoundingBox*				m_pAABB = nullptr;
	BoundingOrientedBox*	m_pOBB_Original = nullptr;
	BoundingOrientedBox*	m_pOBB = nullptr;

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*									m_pEffect = nullptr;
	ID3D11InputLayout*							m_pLayout = nullptr;
	_float4										m_vColor;
#endif

public:
	static CCollider*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, COLLIDERTYPE eType);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END