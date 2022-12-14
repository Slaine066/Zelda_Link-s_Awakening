#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CBareState : public CPlayerState
{
public:
	CBareState();

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;
};
END
END