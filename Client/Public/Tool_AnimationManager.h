#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CTool_AnimationManager final : public CTool
{
private:
	CTool_AnimationManager();
	virtual ~CTool_AnimationManager() = default;

public:
	virtual HRESULT						Initialize(void* pArg) override;
	virtual void							ImGui_RenderWindow() override;

private:
	vector<CGameObject*>*				m_vecAnimObjects = nullptr;

public:
	static CTool_AnimationManager*	Create(void* pArg = nullptr);
	virtual void							Free() override;
};

END