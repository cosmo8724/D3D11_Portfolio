#pragma once
#include "Tool.h"

BEGIN(Client)

class CTestTool final : public CTool
{
private:
	CTestTool();
	virtual ~CTestTool() = default;

public:
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void			ImGui_RenderWindow() override;

private:
	class CMySequencer*	m_pSequencer = nullptr;

public:
	static	CTestTool*	Create(void* pArg = nullptr);
	virtual void			Free() override;
};

END