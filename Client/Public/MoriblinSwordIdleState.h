#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CIdleState : public CMoriblinSwordState
{
public:  
	CIdleState();

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	void Find_Target();

private:
	_float m_fIdleTimer = 0.f;
};
END
END