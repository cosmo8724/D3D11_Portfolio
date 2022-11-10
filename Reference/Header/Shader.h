#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	HRESULT					Initialize_Prototype(const wstring& wstrShderFilePath);
	virtual	HRESULT			Initialize(void* pArg) override;

public:
	static CShader*			Create(DEVICE pDevice, DEVICE_CONTEXT pContext, const wstring& wstrShaderFilPath);
	virtual CComponent*	Clone(void* pArg = nullptr) override;
	virtual void				Free() override;
};

END