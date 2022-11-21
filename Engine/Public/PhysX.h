#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CPhysX final : public CBase
{
private:
	CPhysX();
	virtual ~CPhysX() = default;

public:
	PxScene*		Get_PxScene() const { return m_pScene ? m_pScene : nullptr; }
	PxMaterial*	Get_Material() const { return m_pMaterial ? m_pMaterial : nullptr; }

public:
	HRESULT				Initialize();

public:
	HRESULT				CreateScene(const PxVec3& vGravity);
	HRESULT				CreateSimulation();
	HRESULT				RunSimulation(_double dTimeDelta);
	void					DeleteScene();

private:
	/* Foundation 생성을 위해 필요함. */
	PxDefaultAllocator			m_Allocator;
	PxDefaultErrorCallback		m_ErrorCallback;
	PxFoundation*				m_pFoundation = nullptr;

	/* Physics 생성을 위해 필요함. */
	PxPhysics*					m_pPhysics = nullptr;

	/* CPU 리소스를 효율적으로 공유할 수 있도록 해준다. */
	PxDefaultCpuDispatcher*	m_pDispatcher = nullptr;

	/* PhycX Visual Debugger 연결용 */
	PxPvd*							m_pPVD = nullptr;
	PxPvdTransport*				m_pTransport = nullptr;

	PxScene*						m_pScene = nullptr;
	PxMaterial*					m_pMaterial = nullptr;

public:
	static	CPhysX*	Create();
	virtual void		Free() override;
};

END