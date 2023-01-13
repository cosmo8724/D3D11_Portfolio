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
	class CGameObject*		Get_Owner() const { return m_pOwner; }
	const wstring&			Get_FilePath() const { return m_wstrFilePath; }

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg);
	virtual HRESULT			Render() { return S_OK; }

public:
	virtual void				ImGui_RenderProperty() {}

protected:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

	class CGameObject*		m_pOwner = nullptr;
	wstring					m_wstrFilePath = L"";
	_bool						m_bIsCloned = false;

public:
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END