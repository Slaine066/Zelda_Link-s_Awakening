#pragma once

#include "BossblinState.h"

BEGIN(Client)
BEGIN(Bossblin)
class CTackleState : public CBossblinState
{
public:
	CTackleState(STATETYPE eStateType);

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta);

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;

private:
	void Compute_TargetDirection(CBossblin* pBossblin);
	_bool Tackle(CBossblin* pBossblin, _float fTimeDelta); /* NavigationMesh::CanMove() return value. */
	_bool Should_Break(CBossblin* pBossblin, _float fTimeDelta);
	void BounceBack(CBossblin* pBossblin, _float fTimeDelta);
	void Spawn_ShockwaveEffect(CBossblin* pBossblin);
	void Play_Footsteps(CBossblin* pBossblin);

private:
	_bool m_bDidDamage = false;
	_float3 m_vTargetDirection;
	_float m_fBreakTimer = 0.f;

	_bool m_bIsShockwaveSpawned = false;
	_bool m_bIsSoundPlayed = false;
};
END
END