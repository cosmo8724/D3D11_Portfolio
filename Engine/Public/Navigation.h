#pragma once
#include "Component.h"
#include "Cell.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int		iCurrentIndex = -1;
	} NAVIGATIONDESC;

private:
	CNavigation(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	_uint						Get_CellCount() { return (_uint)m_vecCell.size(); }
	class CCell*				Get_Cell(_int iIndex) { return m_vecCell[iIndex]; }
	CCell::STATE				Get_CurrentCellState() { return m_vecCell[m_tNavigationDesc.iCurrentIndex]->Get_State(); }
	_vector					Get_CellHeight(_float4 vTargetPos);

public:
	virtual HRESULT			Initialize_Prototype(const wstring& wstrFilePath);
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;

public:
	HRESULT					Add_Cell(_float3* vPoints);
	HRESULT					Delete_Cell(_int iIndex);
	HRESULT					Save_Cell(string strSaveFilePath);
	HRESULT					Find_NearBy_Point(_float3& vPoint);
	_bool						IsMoveOnNavigation(_fvector vTargetPos, _float4& vBlockedLine, _float4& vBlockedLineNormal);

//#ifdef _DEBUG
public:
	virtual HRESULT			Render() override;
	HRESULT					Render_Selected_Cell(_int iIndex);
	void						Change_Render_HeightColor(const _float& fHeight, const _float4& vColor);
//#endif // _DEBUG

private:
	vector<class CCell*>		m_vecCell;
	NAVIGATIONDESC		m_tNavigationDesc;

//#ifdef _DEBUG
private:
	class CShader*			m_pShaderCom = nullptr;
//#endif // _DEBUG


private:
	HRESULT					Ready_Neighbor();

public:
	static CNavigation*		Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrFilePath = L"");
	virtual CComponent*	Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void				Free() override;
};

END