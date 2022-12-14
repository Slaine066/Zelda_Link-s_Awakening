#pragma once

#include "BossblinState.h"

BEGIN(Client)
BEGIN(Bossblin)
class CHitState : public CBossblinState
{
public:
	CHitState(_float fDownTimer = 0.f);

	virtual CBossblinState* AI_Behavior(CBossblin* pBossblin) override;
	virtual CBossblinState* Tick(CBossblin* pBossblin, _float fTimeDelta) override;
	virtual CBossblinState* LateTick(CBossblin* pBossblin, _float fTimeDelta);

	virtual void Enter(CBossblin* pBossblin) override;
	virtual void Exit(CBossblin* pBossblin) override;

public:
	_float Get_DownTimer() { return m_fDownTimer; }

private:
	_float m_fDownTimer = 0.f;
};
END
END