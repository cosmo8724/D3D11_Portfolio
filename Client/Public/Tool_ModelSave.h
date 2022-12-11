#pragma once
#include "Client_Define.h"
#include "Tool.h"

BEGIN(Engine)
class CComponent;
class CModel;
END

BEGIN(Client)

class CTool_ModelSave final : public CTool
{
private:
	CTool_ModelSave(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CTool_ModelSave() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				ImGui_RenderWindow() override;

private:
	DEVICE					m_pDevice = nullptr;
	DEVICE_CONTEXT		m_pContext = nullptr;

	CModel*					m_pModel = nullptr;
	CModel*					m_pCloneModel = nullptr;

public:
	static CTool_ModelSave*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, void* pArg = nullptr);
	virtual void				Free() override;
};

END