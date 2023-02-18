#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CNPC abstract : public CGameObject
{
protected:
	CNPC(DEVICE pDevice, DEVICE_CONTEXT pContext);
	CNPC(const CNPC& rhs);
	virtual ~CNPC() = default;

public:
	const _bool&				Get_PlayerDetect() const { return m_bPlayerDetected; }
	void						Set_Player(class CSigrid* pPlayer) { m_pPlayer = pPlayer; }
	void						Set_Talking(_bool bTalking) { m_bNowTalking = bTalking; }
	void						Set_Quest(_bool bQuest) { m_bQuest = bQuest; }
	void						Set_QuestDone(_bool bDone) { m_bQuestDone = bDone; }

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(const wstring& wstrPrototypeTag, void* pArg) override;
	virtual void				Tick(_double dTimeDelta) override;
	virtual void				Late_Tick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void				Collision_Event(class CSigrid* pPlayer) {}
	virtual void				Play_Voice() {};

protected:
	class CSigrid*				m_pPlayer = nullptr;
	_bool						m_bPlayerDetected = false;
	_bool						m_bNowTalking = false;
	_bool						m_bQuest = false;
	_bool						m_bQuestDone = false;
	vector<wstring>			m_vecSentence;

public:
	virtual CGameObject*	Clone(const wstring& wstrPrototypeTag, void* pArg = nullptr) PURE;
	virtual void				Free() override;
};

END