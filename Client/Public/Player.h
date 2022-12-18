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
	virtual void				ImGui_RenderProperty();


private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	

private:
	class CStatic_Camera*	m_pCamera = nullptr;

	_uint						m_iNumWeapons = 0;
	vector<class CWeapon*>	m_vecPlayerWeapon;

	_float						m_fTall = 0.f;
	_float						m_fWeaponHeight = 0.f;
	_float4x4					m_matOffset;
	_float4x4					m_matWeaponOffset;
	_float4x4					m_matHeadWithOffset;

	_double					m_dMouseMove;

private:
	HRESULT					Ready_Weapon();
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

private:
	void						Mouse_Move(_double dTimeDelta);
	void						Move_Camera();
	void						Move_Weapon();

public:
	static CPlayer*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END