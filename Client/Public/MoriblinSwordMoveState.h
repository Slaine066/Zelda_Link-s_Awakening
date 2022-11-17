#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CMoveState : public CMoriblinSwordState
{
public:
	CMoveState(CPlayer* pTarget = nullptr);

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	void Set_MoveTarget(CMoriblinSword* pMoriblinSword);
	void Patrol(CMoriblinSword* pMoriblinSword, _float fTimeDelta);

private:
	_float3 m_vPatrolPosition;
	_bool m_bIsArrived = false;
};
END
END