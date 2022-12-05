#pragma once

#include "MoriblinSpearState.h"

BEGIN(Client)
BEGIN(MoriblinSpear)
class CFallState : public CMoriblinSpearState
{
public:
	CFallState();

	virtual CMoriblinSpearState* AI_Behavior(CMoriblinSpear* pMoriblinSpear) override;
	virtual CMoriblinSpearState* Tick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta) override;
	virtual CMoriblinSpearState* LateTick(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta);

	virtual void Enter(CMoriblinSpear* pMoriblinSpear) override;
	virtual void Exit(CMoriblinSpear* pMoriblinSpear) override;

private:
	void Compute_FallDirection(CMoriblinSpear * pMoriblinSpear);

	/* Function used to:
	- Move (Shift) to center of Tile;
	- Move (Fall) towards Fall Direction. */
	void Move(CMoriblinSpear* pMoriblinSpear, _float fTimeDelta);

private:
	_bool m_bIsFalling = false;
	_float3 m_vFallDirection;
	
	_float m_fShiftingTimer = 0.f;
	_float m_fFallingTimer = 0.f;
};
END
END