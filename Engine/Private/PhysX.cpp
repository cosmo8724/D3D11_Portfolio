#include "..\Public\PhysX.h"

CPhysX::CPhysX()
{
}

HRESULT CPhysX::Initialize()
{
	char*		szTransport = "127.0.0.1";

	m_pFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, m_Allocator, m_ErrorCallback);

	/* Optional : Create PVD */
	m_pPVD = PxCreatePvd(*m_pFoundation);
	m_pTransport = PxDefaultPvdSocketTransportCreate(szTransport, 5425, 10);
	_bool					bPVDConnectionResult = m_pPVD->connect(*m_pTransport, PxPvdInstrumentationFlag::eALL);

	if (!bPVDConnectionResult)
	{
		MSG_BOX("Failed to Create : PhysX Visual Debugger");
		return E_FAIL;
	}
	/* #Create PVD# */

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPVD);
	m_pDispatcher = PxDefaultCpuDispatcherCreate(2);

	return S_OK;
}

HRESULT CPhysX::CreateScene(const PxVec3& vGravity)
{
	PxSceneDesc	SceneDesc(m_pPhysics->getTolerancesScale());

	SceneDesc.gravity = vGravity;
	SceneDesc.cpuDispatcher = m_pDispatcher;

	/* TODO */
	//SceneDesc.simulationEventCallback = this;

	SceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_pScene = m_pPhysics->createScene(SceneDesc);

	PxPvdSceneClient*	pPVDClient = m_pScene->getScenePvdClient();
	if (pPVDClient)
	{
		pPVDClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pPVDClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pPVDClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return S_OK;
}

HRESULT CPhysX::CreateSimulation()
{
	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	PxRigidStatic*	pGroundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1.f, 0, 99.f), *m_pMaterial);
	m_pScene->addActor(*pGroundPlane);


	return S_OK;
}

HRESULT CPhysX::RunSimulation()
{
	m_pScene->simulate(1.f / 165.f);
	m_pScene->fetchResults(true);

	return S_OK;
}

void CPhysX::DeleteScene()
{
	m_pScene->release();
	m_pMaterial->release();
}

CPhysX * CPhysX::Create()
{
	CPhysX*	pInstance = new CPhysX;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CPhysX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysX::Free()
{
	if (m_pMaterial)
		m_pMaterial->release();
	if (m_pScene)
		m_pScene->release();

	m_pDispatcher->release();
	m_pPhysics->release();
	m_pPVD->release();
	m_pTransport->release();
	m_pFoundation->release();
}
