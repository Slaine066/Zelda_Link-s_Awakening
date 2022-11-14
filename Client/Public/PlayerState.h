#pragma once

#include "Engine_Defines.h"
#include "Player.h"

BEGIN(Client)
class CPlayerState
{
public:
	virtual ~CPlayerState() {};
	virtual CPlayerState* HandleInput(CPlayer* pPlayer) { return nullptr; };
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) { return nullptr; };
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) { return nullptr; };

	virtual void Enter(CPlayer* pPlayer) {};
	virtual void Exit(CPlayer* pPlayer) {};

protected:
	_bool m_bIsAnimationFinished = false;
};
END