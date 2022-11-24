#pragma once

#include "BossblinState.h"

/* Spear Throwing Attack. */

BEGIN(Client)
BEGIN(Bossblin)
class CAttackState : public CBossblinState
{
public:
	CAttackState();

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta);

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;

private:
	void Move(CBossblin* pBossblin, _float fTimeDelta);
	void Create_Projectile(CBossblin * pBossblin);
};
END
END