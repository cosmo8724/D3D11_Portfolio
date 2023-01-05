#pragma once
#include "Client_Define.h"
#include "Tool.h"
#include "Cell.h"

BEGIN(Engine)
class CGameInstance;
class CNavigation;
class CVIBuffer_Cell_Circle;
END

BEGIN(Client)

class CTool_Navigation final : public CTool
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };

private:
	CTool_Navigation(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CTool_Navigation() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					ImGui_RenderWindow() override;
	virtual void					Render() override;

private:
	DEVICE						m_pDevice = nullptr;
	DEVICE_CONTEXT			m_pContext = nullptr;

	CGameInstance*				m_pGameInstance = nullptr;

private:
	_uint							m_iCurPoint = 0;
	_float3						m_vPoint[POINT_END] = { _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f) };

	CNavigation*					m_pNavigationCom = nullptr;
	_uint							m_iNumCell = 0;
	_int							m_iSelectedCell = -1;

	char*							m_pStateName[CCell::STATE_END + 1] = { "State_Ocean", "State_Ground", "None" };

#ifdef _DEBUG
	CVIBuffer_Cell_Circle*		m_pVIBufferCircleCom[POINT_END] = { nullptr, nullptr, nullptr };
#endif // _DEBUG


public:
	static CTool_Navigation*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void* pArg = nullptr);
	virtual void					Free() override;
};

END