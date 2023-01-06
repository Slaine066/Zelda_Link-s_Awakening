#pragma once

#include "OwlState.h"

BEGIN(Client)
BEGIN(Owl)
class CFlyOutState : public COwlState
{
public:
	CFlyOutState(STATETYPE eStateType);

	virtual COwlState* AI_Behavior(COwl* pOwl) override;
	virtual COwlState* Tick(COwl* pOwl, _float fTimeDelta) override;
	virtual COwlState* LateTick(COwl* pOwl, _float fTimeDelta) override;

	virtual void Enter(COwl* pOwl) override;
	virtual void Exit(COwl* pOwl) override;
};
END
END