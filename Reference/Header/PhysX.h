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
	/* Foundation ������ ���� �ʿ���. */
	PxDefaultAllocator			m_Allocator;
	PxDefaultErrorCallback		m_ErrorCallback;
	PxFoundation*				m_pFoundation = nullptr;

	/* Physics ������ ���� �ʿ���. */
	PxPhysics*					m_pPhysics = nullptr;

	/* CPU ���ҽ��� ȿ�������� ������ �� �ֵ��� ���ش�. */
	PxDefaultCpuDispatcher*	m_pDispatcher = nullptr;

	/* PhycX Visual Debugger ����� */
	PxPvd*							m_pPVD = nullptr;
	PxPvdTransport*				m_pTransport = nullptr;

	PxScene*						m_pScene = nullptr;
	PxMaterial*					m_pMaterial = nullptr;

public:
	static	CPhysX*	Create();
	virtual void		Free() override;
};

END