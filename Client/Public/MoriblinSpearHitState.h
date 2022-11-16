#pragma once

#include "MoriblinSpearState.h"

BEGIN(Client)
BEGIN(MoriblinSpear)
class CHitState : public CMoriblinSpearState
{
public:
	CHitState(_float3 vDamageCauserPosition);

	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSpear) override;
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) override;
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta);

	virtual void Enter(CMoriblinSpear* pMoriblinSpear) override;
	virtual void Exit(CMoriblinSpear* pMoriblinSpear) override;

private:
	_bool Compute_HitPosition(CMoriblinSpear * pMoriblinSpear);

private:
	_float3 m_vDamageCauserPosition;
};
END
END