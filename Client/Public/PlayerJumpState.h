#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CJumpState : public CPlayerState
{
public:
	CJumpState(STATETYPE eStateType, _bool bMove = false);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	_bool Check_CollisionBlock(CPlayer* pPlayer, _float fTimeDelta);

private:
	_bool m_bMove = false;
	CActor* m_pBlockingObject = nullptr;
};
END
END