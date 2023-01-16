#pragma once
#include "Client_Define.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CStateMachine;
END

BEGIN(Client)

class CSigrid_CutScn final : public CGameObject
{
private:
	CSigrid_CutScn(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CSigrid_CutScn(const CSigrid_CutScn& rhs);
	virtual ~CSigrid_CutScn() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CShader*					m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CStateMachine*			m_pStateMachineCom = nullptr;
	class CSigridCutScn_State*		m_pSigridState = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CSigrid_CutScn*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END