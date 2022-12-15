#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	_matrix					Get_BornMatrix(const string& strBoneName);
	_matrix					Get_OffsetMatrix(const string & strBoneName);
	_matrix					Get_PivotMatrix();
	_vector					Get_TransformState(CTransform::STATE eState);

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	class CStatic_Camera*	m_pCamera = nullptr;

	_uint						m_iNumParts = 0;
	vector<CGameObject*>	m_vecPlayerPart;

	_float						m_fTall = 0.f;

private:
	HRESULT					Ready_Part();
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CPlayer*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END