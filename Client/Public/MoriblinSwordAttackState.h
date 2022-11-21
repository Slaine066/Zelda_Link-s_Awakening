#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CAttackState : public CMoriblinSwordState
{
public:
	CAttackState(_bool bIsHit = false);

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta);

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	void Move(CMoriblinSword* pMoriblinSword, _float fTimeDelta);
	void BounceBack(CMoriblinSword * pMoriblinSpear, _float fTimeDelta);

private:
	_bool m_bIsHit = false;
	_float m_fPushBackTimer = 0.f;
	_bool m_bDidDamage = false;
};
END
END