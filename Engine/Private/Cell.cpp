#include "..\Public\Cell.h"

CCell::CCell(DEVICE pDevice, DEVICE_CONTEXT pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	return S_OK;
}

CCell * CCell::Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Create : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
