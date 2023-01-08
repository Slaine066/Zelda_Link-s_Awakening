#pragma once

#include "MarinState.h"

BEGIN(Client)
BEGIN(Marin)
class CTalkState : public CMarinState
{
public:
	CTalkState();

	virtual CMarinState* AI_Behavior(CMarin* pMarin) override;
	virtual CMarinState* Tick(CMarin* pMarin, _float fTimeDelta) override;
	virtual CMarinState* LateTick(CMarin* pMarin, _float fTimeDelta) override;

	virtual void Enter(CMarin* pMarin) override;
	virtual void Exit(CMarin* pMarin) override;

private:
	_float m_fSoundTimer = 3.f;
	_uint m_iSoundCounter = 0;
};
END
END