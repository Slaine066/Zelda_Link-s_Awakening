#pragma once

#include "TarinState.h"

BEGIN(Client)
BEGIN(Tarin)
class CIdleState : public CTarinState
{
public:
	CIdleState();

	virtual CTarinState* AI_Behavior(CTarin* pTarin) override;
	virtual CTarinState* Tick(CTarin* pTarin, _float fTimeDelta) override;
	virtual CTarinState* LateTick(CTarin* pTarin, _float fTimeDelta) override;

	virtual void Enter(CTarin* pTarin) override;
	virtual void Exit(CTarin* pTarin) override;

private:
	_float m_fIdleMoveTimer = 0.f;
};
END
END