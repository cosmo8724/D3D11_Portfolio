#pragma once
#include "Tool.h"

BEGIN(Client)

class CTestTool final : public CTool
{
private:
	CTestTool();
	virtual ~CTestTool() = default;

public:
	virtual HRESULT		Ready_Tool() override;
	virtual HRESULT		Update_Tool(_double dTimeDelta) override;

private:
	class CMySequencer*	m_pSequencer = nullptr;

public:
	static	CTestTool*	Create();
	virtual void			Free() override;
};

END