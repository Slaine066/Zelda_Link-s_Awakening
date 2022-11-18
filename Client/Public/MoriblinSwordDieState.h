#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CDieState : public CMoriblinSwordState
{
public:
	CDieState(_float3 vDamageCauserPosition);

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta);

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	_bool Compute_HitPosition(CMoriblinSword * pMoriblinSword);
	void BounceBack(CMoriblinSword * pMoriblinSword, _float fTimeDelta);

private:
	_float3 m_vDamageCauserPosition;
	_float m_fDeadTimer = 0.f;
};
END
END