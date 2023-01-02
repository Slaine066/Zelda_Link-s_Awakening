#include "stdafx.h"

#include "TarinIdleState.h"
#include "UI.h"

using namespace Tarin;

CIdleState::CIdleState()
{
}

CTarinState * CIdleState::AI_Behavior(CTarin * pTarin)
{
	return nullptr;
}

CTarinState * CIdleState::Tick(CTarin * pTarin, _float fTimeDelta)
{
	pTarin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pTarin->Is_AnimationLoop(pTarin->Get_Model()->Get_CurrentAnimIndex()));
	pTarin->Sync_WithNavigationHeight();

	return nullptr;
}

CTarinState * CIdleState::LateTick(CTarin * pTarin, _float fTimeDelta)
{
	if (pTarin->CanInteract())
		pTarin->Spawn_InteractButton();
	else
	{
		if (pTarin->Get_InteractButton())
		{
			pTarin->Get_InteractButton()->Set_ShouldDestroy(true);
			pTarin->Set_InteractButton(nullptr);
		}
	}

	return nullptr;
}

void CIdleState::Enter(CTarin * pTarin)
{
	m_eStateId = STATE_ID::STATE_IDLE;

	pTarin->Get_Model()->Set_CurrentAnimIndex(CTarin::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CTarin * pTarin)
{
	m_fIdleMoveTimer = 0.f;
}