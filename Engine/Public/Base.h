#pragma once
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	_ulong		AddRef();
	_ulong		Release();

private:
	_ulong		m_dwRefCnt;

public:
	virtual void Free() PURE;
};

END