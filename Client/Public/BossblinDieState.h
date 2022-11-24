#pragma once

#include "BossblinState.h"

BEGIN(Client)
BEGIN(Bossblin)
class CDieState : public CBossblinState
{
public:
	CDieState(STATETYPE eStateType);

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta);

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;

private:
	_float m_fDeadTimer = 0.f;
};
END
END