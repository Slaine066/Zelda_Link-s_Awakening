#pragma once

#include "MoriblinSpearState.h"

BEGIN(Client)
BEGIN(MoriblinSpear)
class CAggroState : public CMoriblinSpearState
{
public:
	CAggroState();

	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSpear) override;
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) override;
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta);

	virtual void Enter(CMoriblinSpear* pMoriblinSpear) override;
	virtual void Exit(CMoriblinSpear* pMoriblinSpear) override;
};
END
END