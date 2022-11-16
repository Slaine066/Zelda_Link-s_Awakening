#pragma once

#include "PlayerState.h"

BEGIN(Client)
class CHitState : public CPlayerState
{
public:
	CHitState(_float3 vDamageCauserPosition);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	_bool Compute_HitPosition(CPlayer * pPlayer);

private:
	_float3 m_vDamageCauserPosition;
};
END