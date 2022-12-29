#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CCarryMoveState : public CPlayerState
{
public:
	CCarryMoveState(DIRID eDir, _bool bFirstSmokeSpawned = false, _bool bSecondSmokeSpawned = false);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	void Move(CPlayer * pPlayer, _float fTimeDelta);

private:
	DIRID m_eDirection = DIRID::DIR_END;

	/* Smoke Effect Variables */
	_bool m_bFirstSmokeSpawned;
	_bool m_bSecondSmokeSpawned;
};
END
END