#pragma once

#include "Engine_Defines.h"
#include "Marin.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class CMarinState
{
public:
	enum STATE_ID
	{
		STATE_IDLE,
		STATE_WALK,
		STATE_TALK,
		STATE_END,
	};

public:
	virtual ~CMarinState() {};
	virtual CMarinState* AI_Behavior(CMarin* pMarin) { return nullptr; };
	virtual CMarinState* Tick(CMarin* pMarin, _float fTimeDelta) { return nullptr; };
	virtual CMarinState* LateTick(CMarin* pMarin, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CMarin* pMarin) {};
	virtual void Exit(CMarin* pMarin) {};

	CMarinState* ChangeState(CMarin* pMarin, CMarinState* pCurrentState, CMarinState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pMarin);
			Safe_Delete(pCurrentState);
		}

		pCurrentState = pNewState;
		pCurrentState->Enter(pMarin);

		return pCurrentState;
	}

public:
	STATE_ID Get_StateId() { return m_eStateId; }

protected:
	STATE_ID m_eStateId = STATE_END;
	_bool m_bIsAnimationFinished = false;
};
END