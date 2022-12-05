#pragma once

#include "PlayerState.h"

BEGIN(Client)
BEGIN(Player)
class CFallState : public CPlayerState
{
public:
	CFallState();

	virtual CPlayerState* HandleInput(CPlayer* pPlayer) override;
	virtual CPlayerState* Tick(CPlayer* pPlayer, _float fTimeDelta) override;
	virtual CPlayerState* LateTick(CPlayer* pPlayer, _float fTimeDelta) override;

	virtual void Enter(CPlayer* pPlayer) override;
	virtual void Exit(CPlayer* pPlayer) override;

private:
	void Compute_RespawnPosition(CPlayer * pPlayer);

	/* Function used to:
	- Move (Shift) to center of Tile;
	- Move (Fall) towards Fall Direction. */
	void Move(CPlayer * pPlayer, _float fTimeDelta); 

private:
	_float3 m_vFallDirection;
	_float3 m_vRespawnPosition;
	_bool m_bIsFalling = false;

	_float m_fShiftingTimer = 0.f;
	_float m_fFallingTimer = 0.f;
};
END
END