#pragma once

#include "MoriblinSwordState.h"

BEGIN(Client)
BEGIN(MoriblinSword)
class CMoveState : public CMoriblinSwordState
{
public:
	CMoveState();

	virtual CMoriblinSwordState* AI_Behavior(CMoriblinSword* pMoriblinSword) override;
	virtual CMoriblinSwordState* Tick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;
	virtual CMoriblinSwordState* LateTick(CMoriblinSword* pMoriblinSword, _float fTimeDelta) override;

	virtual void Enter(CMoriblinSword* pMoriblinSword) override;
	virtual void Exit(CMoriblinSword* pMoriblinSword) override;

private:
	void Set_MoveTarget(CMoriblinSword* pMoriblinSword);
	void Move(CMoriblinSword* pMoriblinSword, _float fTimeDelta);
	_bool Is_ArrivedToTarget(CMoriblinSword* pMoriblinSword);

private:
	_float3 m_vMoveTarget;
};
END
END