#pragma once

#include "BossblinState.h"

BEGIN(Client)
BEGIN(Bossblin)
class CMoveState : public CBossblinState
{
public:
	CMoveState(CPlayer* pTarget = nullptr);

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta) override;

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;

private:
	void Follow_Target(CBossblin* pBossblin, _float fTimeDelta);
};
END
END