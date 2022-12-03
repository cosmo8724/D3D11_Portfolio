#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCustomGameObject final : public CGameObject
{
public:
	enum LEVEL { LEVEL_LOADING, LEVEL_LOGO, LEVEL_TESTSTAGE, LEVEL_END };

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
	virtual HRESULT			Initialize_Prototype(const vector<wstring>& vecPrototypeTags, _uint iNumTextureCom);
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	wstring					m_wstrRendererComTag = L"";
	wstring					m_wstrVIBufferComTag = L"";
	wstring					m_wstrShaderComTag = L"";
	wstring					m_wstrProtoTransformComTag = L"";
	wstring					m_wstrModelComTag = L"";
	_uint						m_iNumTextureCom = 1;
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
	static CCustomGameObject*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const vector<wstring>& vecPrototypeTags, _uint iNumTextureCom = 0);
	virtual CGameObject*			Clone(void* pArg = nullptr) override;
	virtual void						Free() override;
};

END