#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT			Initialize_Prototype(const wstring& wstrFilePath);
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;

private:
	_int						m_iCurrentIndex = 0;
	vector<class CCell*>		m_vecCell;

public:
	static CNavigation*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrFilePath);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END