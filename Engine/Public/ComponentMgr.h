#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Terrain.h"
#include "Model.h"

BEGIN(Engine)

class CComponentMgr final : public CBase
{
	DECLARE_SINGLETON(CComponentMgr)

public:
	CComponentMgr();
	virtual ~CComponentMgr() = default;

public:
	HRESULT					Reserve_Manager(_uint iNumLevels);
	HRESULT					Add_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag, CComponent * pPrototype);
	HRESULT					Clear_Prototype(_uint iLevelIndex);
	class CComponent*		Clone_Component(_uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg = nullptr);

private:
	_uint						m_iNumLevels = 0;

	map<const wstring, class CComponent*>*				m_mapPrototype = nullptr;
	typedef map<const wstring, class CComponent*>	PROTOTYPES;

private:
	class CComponent*		Find_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag);

public:
	virtual void				Free() override;
};

END