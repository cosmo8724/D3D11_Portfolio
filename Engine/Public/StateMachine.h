#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStateMachine final : public CComponent
{
protected:
	typedef struct tagState
	{
		std::function<void(_double)>	State_Start = nullptr;
		std::function<void(_double)>	State_Tick = nullptr;
		std::function<void(_double)>	State_End = nullptr;
	} STATE;

	typedef struct tagChanger
	{
		wstring							wstrNextState = L"";
		std::function<_bool(void)>		Changer_Func = nullptr;
	} CHANGER;

protected:
	CStateMachine(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CStateMachine(const CStateMachine& rhs);
	virtual ~CStateMachine() = default;

public:
	const wstring&			Get_LastState() { return m_wstrLastStateName; }
	const wstring&			Get_CurrentState() { return m_wstrCurrentStateName; }
	const wstring&			Get_NextState() { return m_wstrNextStateName; }

public:
	CStateMachine&			Set_Root(const wstring& wstrStateName);
	CStateMachine&			Add_State(const wstring& wstrStateName);
	CStateMachine&			Finish_Setting() { m_wstrCurrentStateName = m_wstrRootStateName; return *this; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(class CGameObject* pOwner, void* pArg) override;
	virtual void				Tick(_double& dTimeDelta);

public:
	template<typename T>
	CStateMachine& Init_Start(T* Obj, void (T::*memFunc)(_double))
	{
		const auto iter = find_if(m_mapState.begin(), m_mapState.end(), CTag_Finder(m_wstrCurrentStateName));
		assert(iter != m_mapState.end());

		iter->second.State_Start = [Obj, memFunc](_double dTimeDelta)
		{
			(Obj->*memFunc)(dTimeDelta);
		};

		return *this;
	}

	template<typename T>
	CStateMachine&	Init_Tick(T* Obj, void (T::*memFunc)(_double))
	{
		const auto iter = find_if(m_mapState.begin(), m_mapState.end(), CTag_Finder(m_wstrCurrentStateName));
		assert(iter != m_mapState.end());

		iter->second.State_Tick = [Obj, memFunc](_double dTimeDelta)
		{
			(Obj->*memFunc)(dTimeDelta);
		};

		return *this;
	}

	template<typename T>
	CStateMachine&	Init_End(T* Obj, void (T::*memFunc)(_double))
	{
		const auto iter = find_if(m_mapState.begin(), m_mapState.end(), CTag_Finder(m_wstrCurrentStateName));
		assert(iter != m_mapState.end());

		iter->second.State_End = [Obj, memFunc](_double dTimeDelta)
		{
			(Obj->*memFunc)(dTimeDelta);
		};

		return *this;
	}

	template<typename T>
	CStateMachine&	Init_Changer(const wstring& wstrNextState, T* Obj, _bool(T::*memFunc)())
	{
		const auto iter = find_if(m_mapChanger.begin(), m_mapChanger.end(), CTag_Finder(m_wstrCurrentStateName));

		CHANGER		tChanger;
		//ZeroMemory(&tChanger, sizeof(CHANGER));

		tChanger.wstrNextState = wstrNextState;
		tChanger.Changer_Func = [Obj, memFunc]()
		{
			return (Obj->*memFunc)();
		};

		if (iter == m_mapChanger.end())
			m_mapChanger.emplace(m_wstrCurrentStateName, list<CHANGER>{tChanger});
		else
			iter->second.push_back(tChanger);

		return *this;
	}

private:
	map<const wstring, STATE>				m_mapState;
	wstring									m_wstrRootStateName = L"";
	wstring									m_wstrCurrentStateName = L"";
	wstring									m_wstrNextStateName = L"";
	wstring									m_wstrLastStateName = L"";
		
private:
	map<const wstring, list<CHANGER>>	m_mapChanger;

public:
	static CStateMachine*					Create(DEVICE pDevice, DEVICE_CONTEXT pContext);
	virtual CComponent*					Clone(class CGameObject* pOwner, void* pArg = nullptr) override;
	virtual void								Free() override;
};

END