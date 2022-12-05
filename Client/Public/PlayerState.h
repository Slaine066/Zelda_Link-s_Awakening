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
		STATE_IDLE,
		STATE_MOVE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_GUARD,
		STATE_GUARD_MOVE,
		STATE_FALL,
		STATE_DIE,
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
};
END