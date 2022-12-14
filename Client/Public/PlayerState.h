#pragma once

#include "Engine_Defines.h"
#include "Player.h"

BEGIN(Client)
class CPlayerState
{
public:
	enum STATETYPE 
	{ 
		STATETYPE_START, 
		STATETYPE_MAIN,
		STATETYPE_END,
		STATETYPE_DEFAULT
	};

	enum STATE_ID
	{
		STATE_BARE,
		STATE_BARE_MOVE,
		STATE_IDLE,
		STATE_MOVE,
		STATE_JUMP,
		STATE_ATTACK,
		STATE_HIT,
		STATE_GUARD,
		STATE_GUARD_MOVE,
		STATE_CARRY,
		STATE_CARRY_MOVE,
		STATE_THROW,
		STATE_PUSH,
		STATE_PULL,
		STATE_ACHIEVE,
		STATE_FALL,
		STATE_DIE,
		STATE_SLEEP,
		STATE_WAKEUP,
		STATE_OCARINA,
		STATE_END
	};

public:
	virtual ~CPlayerState() {};
	virtual CPlayerState* HandleInput(CPlayer* pPlayer) { return nullptr; };
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) { return nullptr; };
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CPlayer* pPlayer) {};
	virtual void Exit(CPlayer* pPlayer) {};

	CPlayerState* ChangeState(CPlayer* pPlayer, CPlayerState* pCurrentState, CPlayerState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pPlayer);
			Safe_Delete(pCurrentState);
		}
		
		pCurrentState = pNewState;
		pCurrentState->Enter(pPlayer);
		
		return pCurrentState;
	}

public:
	STATE_ID Get_StateId() { return m_eStateId; }

protected:
	STATETYPE m_eStateType = STATETYPE_DEFAULT;
	STATE_ID m_eStateId = STATE_END;
	_bool m_bIsAnimationFinished = false;
	class CInteractableObject* m_pInteractableObject = nullptr;
	class CNpc* m_pNpc = nullptr;
	class CMonster* m_pMonster = nullptr;
};
END