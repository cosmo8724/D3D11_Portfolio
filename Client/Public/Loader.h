#pragma once
#include "Base.h"
#include "Client_Define.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual ~CLoader() = default;

public:
	const LEVEL&			Get_NextLevel() const		{ return m_eNextLevel; }
	const wstring&		Get_LoadingText() const	{ return m_wstrLoadingText; }
	const wstring&		Get_ProtoComFilePath() const { return m_wstrProtoComFilePath; }
	const wstring&		Get_ProtoObjFilePath() const { return m_wstrProtoObjFilePath; }
	CRITICAL_SECTION	Get_CriticalSection() const { return m_CriticalSection; }
	const _bool&			IsFinished() const			{ return m_bIsFinished; }

public:
	HRESULT			Initialize(LEVEL eNextLevel, const wstring& wstrProtoComFilePath, const wstring& wstrProtoObjFilePath);
	HRESULT			Loading_ForLogo(const wstring& wstrProtoComFilePath, const wstring& wstrProtoObjFilePath);
	HRESULT			Loading_ForGamePlay(const wstring& wstrProtoComFilePath, const wstring& wstrProtoObjFilePath);

private:
	DEVICE				m_pDevice = nullptr;
	DEVICE_CONTEXT	m_pContext = nullptr;
	CRITICAL_SECTION	m_CriticalSection;

private:
	LEVEL					m_eNextLevel = LEVEL_END;
	HANDLE				m_hThread;
	_bool					m_bIsFinished = false;
	wstring				m_wstrLoadingText = L"";

	wstring				m_wstrProtoComFilePath = L"";
	wstring				m_wstrProtoObjFilePath = L"";

public:
	static	CLoader*	Create(DEVICE pDevice, DEVICE_CONTEXT pContext, LEVEL eNextLevel, const wstring& wstrProtoComFilePath, const wstring& wstrProtoObjFilePath);
	virtual void		Free() override;
};

END