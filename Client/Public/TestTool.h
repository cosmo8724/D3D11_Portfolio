#pragma once
#include "Tool.h"

class CTestTool final : public CTool
{
private:
	CTestTool();
	virtual ~CTestTool() = default;

public:
	virtual HRESULT		Ready_Tool() override;
	virtual HRESULT		Update_Tool(_double dTimeDelta) override;

public:
	static	CTestTool*	Create();
	virtual void			Free() override;
};

