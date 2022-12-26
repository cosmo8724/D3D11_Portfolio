#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentIndex = -1;
	} NAVIGATIONDESC;

private:
	CNavigation(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT			Initialize_Prototype(const wstring& wstrFilePath);
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	_bool						IsMoveOnNavigation(_fvector vTargetPos);

#ifdef _DEBUG
public:
	HRESULT					Render();
#endif // _DEBUG

private:
	vector<class CCell*>		m_vecCell;
	NAVIGATIONDESC		m_tNavigationDesc;

#ifdef _DEBUG
private:
	class CShader*			m_pShaderCom = nullptr;
#endif // _DEBUG


private:
	HRESULT					Ready_Neighbor();

public:
	static CNavigation*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrFilePath);
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END