#include "stdafx.h"

#include "MarinIdleState.h"
#include "MarinTalkState.h"
#include "UI.h"

using namespace Marin;

CIdleState::CIdleState()
{
}

CMarinState * CIdleState::AI_Behavior(CMarin * pMarin)
{
	return nullptr;
}

CMarinState * CIdleState::Tick(CMarin * pMarin, _float fTimeDelta)
{
	pMarin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMarin->Is_AnimationLoop(pMarin->Get_Model()->Get_CurrentAnimIndex()));
	pMarin->Sync_WithNavigationHeight();

	return nullptr;
}

CMarinState * CIdleState::LateTick(CMarin * pMarin, _float fTimeDelta)
{
	if (pMarin->CanInteract())
		pMarin->Spawn_InteractButton();
	else
	{
		if (pMarin->Get_InteractButton())
		{
			pMarin->Get_InteractButton()->Set_ShouldDestroy(true);
			pMarin->Set_InteractButton(nullptr);
		}
	}

	if (pMarin->Get_CurrentChat())
		return new CTalkState();

	return nullptr;
}

void CIdleState::Enter(CMarin * pMarin)
{
	m_eStateId = STATE_ID::STATE_IDLE;

	pMarin->Get_Model()->Set_CurrentAnimIndex(CMarin::ANIMID::ANIM_WAIT);
}

void CIdleState::Exit(CMarin * pMarin)
{
	if (pMarin->Get_InteractButton())
	{
		pMarin->Get_InteractButton()->Set_ShouldDestroy(true);
		pMarin->Set_InteractButton(nullptr);
	}
}