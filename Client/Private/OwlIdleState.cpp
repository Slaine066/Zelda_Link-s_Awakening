#include "stdafx.h"

#include "OwlIdleState.h"
#include "OwlTalkState.h"
#include "UI.h"

using namespace Owl;

CIdleState::CIdleState()
{
}

COwlState * CIdleState::AI_Behavior(COwl * pOwl)
{
	return nullptr;
}

COwlState * CIdleState::Tick(COwl * pOwl, _float fTimeDelta)
{
	pOwl->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pOwl->Is_AnimationLoop(pOwl->Get_Model()->Get_CurrentAnimIndex()));

	return nullptr;
}

COwlState * CIdleState::LateTick(COwl * pOwl, _float fTimeDelta)
{
	if (pOwl->CanInteract())
		pOwl->Spawn_InteractButton();
	else
	{
		if (pOwl->Get_InteractButton())
		{
			pOwl->Get_InteractButton()->Set_ShouldDestroy(true);
			pOwl->Set_InteractButton(nullptr);
		}
	}

	if (pOwl->Get_CurrentChat())
		return new CTalkState();

	return nullptr;
}

void CIdleState::Enter(COwl * pOwl)
{
	m_eStateId = STATE_ID::STATE_IDLE;

	pOwl->Get_Model()->Set_CurrentAnimIndex(COwl::ANIMID::ANIM_IDLE);
}

void CIdleState::Exit(COwl * pOwl)
{
	if (pOwl->Get_InteractButton())
	{
		pOwl->Get_InteractButton()->Set_ShouldDestroy(true);
		pOwl->Set_InteractButton(nullptr);
	}
}