#pragma once

#include "PlayerState.h"
#include "Bed.h"

BEGIN(Client)
BEGIN(Player)
class CSleepState : public CPlayerState
{
public:
	CSleepState();

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	CBed* m_pBed = nullptr;
	_bool m_bIsSleeping = false; /* m_bIsBedSleeping */
};
END
END