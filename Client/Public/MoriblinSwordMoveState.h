#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CMoveState : public CMoriblinSwordState
{
public:
	CMoveState(DIRID eDir);

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	DIRID m_eDirection = DIRID::DIR_END;
};
END
END