#pragma once

#include "Engine_Defines.h"
#include "Owl.h"
#include "GameInstance.h"
#include "Player.h"

BEGIN(Client)
class COwlState
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
		STATE_FLY_ONPLACE,
		STATE_FLY_IN,
		STATE_FLY_OUT,
		STATE_FLY_TALK,
		STATE_HIDDEN,
		STATE_SERIOUS_TALK,
		STATE_TALK,
		STATE_END,
	};

public:
	virtual ~COwlState() {};
	virtual COwlState* AI_Behavior(COwl* pOwl) { return nullptr; };
	virtual COwlState* Tick(COwl* pOwl, _float fTimeDelta) { return nullptr; };
	virtual COwlState* LateTick(COwl* pOwl, _float fTimeDelta) { return nullptr; };

	virtual void Enter(COwl* pOwl) {};
	virtual void Exit(COwl* pOwl) {};

	COwlState* ChangeState(COwl* pOwl, COwlState* pCurrentState, COwlState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pOwl);
			Safe_Delete(pCurrentState);
		}

		pCurrentState = pNewState;
		pCurrentState->Enter(pOwl);

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