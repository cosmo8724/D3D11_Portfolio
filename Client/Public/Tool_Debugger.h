#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Client)

class CTool_Debugger : public CTool
{
private:
	CTool_Debugger() = default;
	virtual ~CTool_Debugger() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				ImGui_RenderWindow() override;

public:
	static CTool_Debugger*	Create(void* pArg = nullptr);
	virtual void				Free() override;
};

END