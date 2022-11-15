#include "stdafx.h"

#include "MoriblinSwordHitState.h"

using namespace MoriblinSword;

CHitState::CHitState(_float3 vDamageCauserPosition) : m_vDamageCauserPosition(vDamageCauserPosition)
{
}

CMoriblinSwordState * CHitState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CHitState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

CMoriblinSwordState * CHitState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	return nullptr;
}

void CHitState::Enter(CMoriblinSword * pMoriblinSword)
{
	_bool bIsFront = Compute_HitPosition();
	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(bIsFront ? CMoriblinSword::ANIMID::ANIM_DAMAGE_FRONT : CMoriblinSword::ANIMID::ANIM_DAMAGE_BACK);
}

void CHitState::Exit(CMoriblinSword * pMoriblinSword)
{
}

_bool CHitState::Compute_HitPosition()
{
	if (true)
	{
		// TODO: ..
		return true;
	}
	else
		return false;
}
