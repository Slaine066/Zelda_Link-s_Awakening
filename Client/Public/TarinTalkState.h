#pragma once

#include "TarinState.h"

BEGIN(Client)
BEGIN(Tarin)
class CTalkState : public CTarinState
{
public:
	CTalkState();

	virtual CTarinState* AI_Behavior(CTarin* pTarin) override;
	virtual CTarinState* Tick(CTarin* pTarin, _float fTimeDelta) override;
	virtual CTarinState* LateTick(CTarin* pTarin, _float fTimeDelta) override;

	virtual void Enter(CTarin* pTarin) override;
	virtual void Exit(CTarin* pTarin) override;

private:
	_float m_fSoundTimer = 3.f;
	_uint m_iSoundCounter = 0;
};
END
END