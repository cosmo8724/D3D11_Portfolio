#pragma once
#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	HRESULT			Save_Bone(HANDLE& hFile, DWORD& dwByte);
	HRESULT			Save_BoneName(HANDLE& hFile, DWORD& dwByte);
	HRESULT			Load_Bone(HANDLE& hFile, DWORD& dwByte);
	const string&		Get_Name() const { return m_strName; }
	const _uint&		Get_NumChiled() const { return m_iNumChild; }
	_matrix			Get_matOffset() { return XMLoadFloat4x4(&m_matOffset); }
	_matrix			Get_CombindMatrix() { return XMLoadFloat4x4(&m_matCombindTransform); }
	_matrix			Get_matTransform() { return XMLoadFloat4x4(&m_matTransform); }
	void				Set_matOffset(_float4x4 matOffset) { m_matOffset = matOffset; }
	void				Set_matTransform(_fmatrix matTransform) { XMStoreFloat4x4(&m_matTransform, matTransform); }

public:
	HRESULT			Initialize(aiNode* pAINode, CBone* pParent, _uint iNumChild);
	void				Compute_CombindTransformMatrix();

private:
	string				m_strName = "";

	_float4x4			m_matOffset;
	_float4x4			m_matTransform;
	_float4x4			m_matCombindTransform;

	CBone*			m_pParent = nullptr;
	_uint				m_iNumChild = 0;

public:
	static CBone*		Create(aiNode* pAINode, CBone* pParent, _uint iNumChild);
	virtual void		Free() override;
};

END