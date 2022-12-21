#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CMoveState : public CPlayerState
{
public:
	CMoveState(DIRID eDir, _bool bFirstSmokeSpawned = false, _bool bSecondSmokeSpawned = false);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta);

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	void Move(CPlayer * pPlayer, _float fTimeDelta);
	_bool Check_CollisionBlock(CPlayer* pPlayer, _float fTimeDelta);
	
private:
	DIRID m_eDirection = DIRID::DIR_END;
	CActor* m_pBlockingObject = nullptr;

	/* Smoke Effect Variables */
	_bool m_bFirstSmokeSpawned;
	_bool m_bSecondSmokeSpawned;
};
END
END