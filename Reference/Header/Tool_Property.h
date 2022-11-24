#pragma once
#include "Tool.h"

BEGIN(Engine)

class ENGINE_DLL CTool_Property final : public CTool
{
private:
	CTool_Property() = default;
	virtual ~CTool_Property() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;

	virtual void				ImGui_RenderTap() override;
	virtual void				ImGui_RenderWindow() override;

private:
	class CGameObject*		m_pSelectedObject = nullptr;

public:
	static CTool_Property*	Create(void* pArg = nullptr);
	virtual void				Free() override;
};

END