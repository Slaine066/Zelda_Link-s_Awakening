#pragma once

#include "Engine_Defines.h"
#include "MoriblinSword.h"

BEGIN(Client)
class CMoriblinSwordState
{
public:
	virtual ~CMoriblinSwordState() {};
	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) { return nullptr; };
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) { return nullptr; };
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CMoriblinSword* pMoriblinSword) {};
	virtual void Exit(CMoriblinSword* pMoriblinSword) {};

	CMoriblinSwordState* ChangeState(CMoriblinSword* pMoriblinSword, CMoriblinSwordState* pCurrentState, CMoriblinSwordState* pNewState)
	{
		if (pCurrentState)
		{
			pCurrentState->Exit(pMoriblinSword);
			Safe_Delete(pCurrentState);
		}

		pNewState->Enter(pMoriblinSword);
		return pNewState;
	}

protected:
	_bool m_bIsAnimationFinished = false;
};
END