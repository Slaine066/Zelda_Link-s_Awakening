#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CFallState : public CMoriblinSwordState
{
public:
	CFallState();

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta);

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	void Compute_FallDirection(CMoriblinSword * pMoriblinSword);

	/* Function used to:
	- Move (Shift) to center of Tile;
	- Move (Fall) towards Fall Direction. */
	void Move(CMoriblinSword* pMoriblinSword, _float fTimeDelta);

private:
	_bool m_bIsFalling = false;
	_float3 m_vFallDirection;

	_float m_fShiftingTimer = 0.f;
	_float m_fFallingTimer = 0.f;
};
END
END