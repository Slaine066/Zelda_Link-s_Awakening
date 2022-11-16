#pragma once

#include "MoriblinSpearState.h"

BEGIN(Client)
BEGIN(MoriblinSpear)
class CMoveState : public CMoriblinSpearState
{
public:
	CMoveState();

	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSpear) override;
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) override;
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) override;

	virtual void Enter(CMoriblinSpear* pMoriblinSpear) override;
	virtual void Exit(CMoriblinSpear* pMoriblinSpear) override;

private:
	void Set_MoveTarget(CMoriblinSpear* pMoriblinSpear);
	void Move(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta);

private:
	_float3 m_vMoveTarget;
	_bool m_bIsArrived = false;
};
END
END