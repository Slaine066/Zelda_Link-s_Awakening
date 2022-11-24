#pragma once

#include "BossblinState.h"

BEGIN(Client)
BEGIN(Bossblin)
class CBreakState : public CBossblinState
{
public:
	CBreakState();

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta);

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;

private:
	void Break(CBossblin* pBossblin, _float fTimeDelta);
};
END
END