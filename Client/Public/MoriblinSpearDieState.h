#pragma once

#include "MoriblinSpearState.h"

BEGIN(Client)
BEGIN(MoriblinSpear)
class CDieState : public CMoriblinSpearState
{
public:
	CDieState(_float3 vDamageCauserPosition);

	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSpear) override;
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) override;
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta);

	virtual void Enter(CMoriblinSpear* pMoriblinSpear) override;
	virtual void Exit(CMoriblinSpear* pMoriblinSpear) override;

private:
	_bool Compute_HitPosition(CMoriblinSpear * pMoriblinSpear);

private:
	_float3 m_vDamageCauserPosition;
	_float m_fDeadTimer = 0.f;
};
END
END