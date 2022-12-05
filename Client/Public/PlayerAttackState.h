#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CAttackState : public CPlayerState
{
public:
	CAttackState();

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta);

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	_bool m_bDidDamage = false;
};
END
END