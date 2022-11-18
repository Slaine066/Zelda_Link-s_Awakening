#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CAggroState : public CMoriblinSwordState
{
public:
	CAggroState();

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta);

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;
};
END
END