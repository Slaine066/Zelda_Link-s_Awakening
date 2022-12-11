#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CPushState : public CPlayerState
{
public:
	CPushState(STATETYPE eStateType, DIRID eDir = DIR_END, CActor* pPushedObject = nullptr);

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	CActor* m_pPushedObject = nullptr;
	DIRID m_eDirection = DIRID::DIR_END;
};
END
END