#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Client)

class CTool_Settings final : public CTool
{
private:
	CTool_Settings() = default;
	virtual ~CTool_Settings() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				ImGui_RenderWindow() override;

private:
	_float*					m_pAlpha = nullptr;

public:
	static CTool_Settings*	Create(void* pArg = nullptr);
	virtual void				Free() override;
};

END