#pragma once

#include "Engine_Defines.h"
#include "Tarin.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class CTarinState
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
	virtual ~CTarinState() {};
	virtual CTarinState* AI_Behavior(CTarin* pTarin) { return nullptr; };
	virtual CTarinState* Tick(CTarin* pTarin, _float fTimeDelta) { return nullptr; };
	virtual CTarinState* LateTick(CTarin* pTarin, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CTarin* pTarin) {};
	virtual void Exit(CTarin* pTarin) {};

	CTarinState* ChangeState(CTarin* pTarin, CTarinState* pCurrentState, CTarinState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pTarin);
			Safe_Delete(pCurrentState);
		}

		pCurrentState = pNewState;
		pCurrentState->Enter(pTarin);

		return pCurrentState;
	}

public:
	STATE_ID Get_StateId() { return m_eStateId; }

protected:
	STATE_ID m_eStateId = STATE_END;
	_bool m_bIsAnimationFinished = false;
};
END