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
	void				Set_matOffset(_float4x4 matOffset) { m_matOffset = matOffset; }

public:
	HRESULT			Initialize(aiNode* pAINode);
	void				Compute_CombindTransformMatrix();

private:
	string				m_strName = "";

	_float4x4			m_matOffset;
	_float4x4			m_matTransform;
	_float4x4			m_matCombindTransform;

	CBone*			m_pParent;

public:
	static CBone*		Create(aiNode* pAINode);
	virtual void		Free() override;
};

END