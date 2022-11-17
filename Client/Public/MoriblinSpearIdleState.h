#pragma once

#include "MoriblinSpearState.h"

BEGIN(Client)
BEGIN(MoriblinSpear)
class CIdleState : public CMoriblinSpearState
{
public:
	CIdleState(CPlayer* pTarget = nullptr);

	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSword) override;
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSword, _float fTimeDelta) override;

	virtual void Enter(CMoriblinSpear* pMoriblinSword) override;
	virtual void Exit(CMoriblinSpear* pMoriblinSword) override;

private:
	_float m_fIdleMoveTimer = 0.f;
};
END
END