#pragma once

#include "BossblinState.h"

BEGIN(Client)
BEGIN(Bossblin)
class CGuardState : public CBossblinState
{
public:
	CGuardState();

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta);

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;
};
END
END