#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCustomGameObject final : public CGameObject
{
public:
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_TESTSTAGE, LEVEL_END };
	enum OBJECTUSAGE { PLAYER, MONSTER, UI, INVENTORY, COMMONOBJECT, OBJECTUSAGE_END };

	typedef struct tagProtoTypeInfos
	{
		// VIBuffer Type
		// Shader Type
	} PROTOTYPEINFOS;

private:
	CCustomGameObject(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CCustomGameObject(const CCustomGameObject& rhs);
	virtual ~CCustomGameObject() = default;

public:
	vector<pair<_uint, wstring>>	Get_PrototypeSaveData();
	const _uint&						Get_TextureComCount() const { return m_iNumTextureCom; }
	class CModel*						Get_ModelComponent() const { return m_pModelCom; }

public:
	virtual HRESULT			Initialize_Prototype(const vector<pair<_uint, wstring>>& vecPrototypeInfo, _uint iNumTextureCom);
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_uint						m_iRendererComLevel = 1000;
	_uint						m_iVIBufferComLevel = 1000;
	_uint						m_iShaderComLevel = 1000;
	_uint						m_iTransformComLevel = 1000;
	_uint						m_iTextureComLevel = 1000;
	_uint						m_iModelComLevel = 1000;

	wstring					m_wstrRendererComTag = L"";
	wstring					m_wstrVIBufferComTag = L"";
	wstring					m_wstrShaderComTag = L"";
	wstring					m_wstrProtoTransformComTag = L"";
	wstring					m_wstrModelComTag = L"";
	_uint						m_iNumTextureCom = 0;
	wstring*					m_wstrTextureComTag = nullptr;

	class CRenderer*			m_pRendererCom = nullptr;
	class CVIBuffer*			m_pVIBufferCom = nullptr;
	class CShader*			m_pShaderCom = nullptr;
	class CTexture**			m_pTextureCom = nullptr;
	class CModel*				m_pModelCom = nullptr;

private:
	HRESULT					SetUp_Component();
	HRESULT					SetUp_ShaderResource();

public:
	static CCustomGameObject*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const vector<pair<_uint, wstring>>& vecPrototypeInfo, _uint iNumTextureCom = 0);
	virtual CGameObject*			Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) override;
	virtual void						Free() override;
};

END