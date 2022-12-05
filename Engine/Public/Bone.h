#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const string&		Get_Name() const { return m_strName; }
	_matrix			Get_matOffset() { return XMLoadFloat4x4(&m_matOffset); }
	_matrix			Get_CombindMatrix() { return XMLoadFloat4x4(&m_matCombindTransform); }
	void				Set_matOffset(_float4x4 matOffset) { m_matOffset = matOffset; }
	void				Set_matTransform(_fmatrix matTransform) { XMStoreFloat4x4(&m_matTransform, matTransform); }

public:
	HRESULT			Initialize(aiNode* pAINode, CBone* pParent);
	void				Compute_CombindTransformMatrix();

private:
	string				m_strName = "";

	_float4x4			m_matOffset;
	_float4x4			m_matTransform;
	_float4x4			m_matCombindTransform;

	CBone*			m_pParent = nullptr;

public:
	static CBone*		Create(aiNode* pAINode, CBone* pParent);
	virtual void		Free() override;
};

END