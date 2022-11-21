#pragma once

#include "PlayerState.h"

BEGIN(Client)
class CGuardMoveState : public CPlayerState
{
public:
	CGuardMoveState(DIRID eDir);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta);

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	void Move(CPlayer * pPlayer, _float fTimeDelta);

private:
	DIRID m_eDirection = DIRID::DIR_END;
};
END