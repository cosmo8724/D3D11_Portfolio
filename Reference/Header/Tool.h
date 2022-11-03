#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTool abstract : public CBase
{
protected:
	CTool();
	virtual ~CTool() = default;

public:
	virtual HRESULT	Ready_Tool() PURE;
	virtual	HRESULT	Update_Tool(_double dTimeDelta) PURE;

protected:
	_double	m_dTimeDelta = 0.0;

public:
	virtual void		Free() PURE;
};

END