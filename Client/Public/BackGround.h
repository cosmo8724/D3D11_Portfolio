#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)

class CRenderer;
class CShader;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CBackGround final : public CGameObject
{
private:
	CBackGround(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

private:
	HRESULT					SetUp_Component();

public:
	static CBackGround*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END