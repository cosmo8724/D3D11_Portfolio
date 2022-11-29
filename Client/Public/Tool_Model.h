#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Client)

class CTool_Model final : public CTool
{
private:
	CTool_Model();
	virtual ~CTool_Model() = default;

public:
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void					ImGui_RenderTap() override;

public:
	static CTool_Model*			Create(void* pArg = nullptr);
	virtual void					Free() override;
};

END