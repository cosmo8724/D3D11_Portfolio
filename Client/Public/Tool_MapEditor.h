#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Client)

class CTool_MapEditor final : public CTool
{
private:
	CTool_MapEditor();
	virtual ~CTool_MapEditor();

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					ImGui_RenderTap() override;

public:
	static CTool_MapEditor*	Create(void* pArg = nullptr);
	virtual void					Free() override;
};

END