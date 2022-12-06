#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	const wstring&			Get_FilePath() const { return m_wstrFilePath; }

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);

public:
	virtual void				ImGui_RenderProperty() {}

protected:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

	wstring					m_wstrFilePath = L"";

	_bool						m_bIsCloned = false;

public:
	virtual CComponent*	Clone(void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END