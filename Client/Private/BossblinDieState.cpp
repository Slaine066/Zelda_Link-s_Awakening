#include "stdafx.h"

#include "BossblinDieState.h"

using namespace Bossblin;

CDieState::CDieState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CBossblinState * CDieState::AI_Behavior(CBossblin * pBossblin)
{
	return nullptr;
}

CBossblinState * CDieState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	switch (m_eStateType)
	{
		case STATETYPE_MAIN:
			if (m_fDeadTimer > pBossblin->Get_DissolveLifespanTimer())
				pBossblin->Set_ShouldDestroy(true);
			else
			{
				m_fDeadTimer += fTimeDelta;
				pBossblin->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_DISSOLVE);
			}
			break;
	}

	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	return nullptr;
}

CBossblinState * CDieState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CDieState(STATETYPE_MAIN);
			break;
		}
	}

	return nullptr;
}

void CDieState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_DIE;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pBossblin->Set_Dead(true);
		pBossblin->Set_ShaderPass(VTXANIMMODELPASS::VTXANIMMODEL_HIT);
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DEAD_START);
		break;
	case STATETYPE_MAIN:
		pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_DEAD);
		break;
	}
}

void CDieState::Exit(CBossblin * pBossblin)
{
}