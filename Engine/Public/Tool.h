#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTool abstract : public CBase
{
protected:
	CTool() = default;
	virtual ~CTool() = default;

public:
	const char*		Get_TabName() const { return m_szTabName; }
	const char*		Get_WindowName() const { return m_szWIndowName; }

public:
	virtual HRESULT	Initialize(void* pArg);

	virtual void		ImGui_RenderTap() {}
	virtual void		ImGui_RenderWindow() {}
	virtual void		Render() {}

protected:
	_double			m_dTimeDelta = 0.0;
	const char*		m_szTabName = "";
	const char*		m_szWIndowName = "";

public:
	virtual void		Free() PURE;
};

END