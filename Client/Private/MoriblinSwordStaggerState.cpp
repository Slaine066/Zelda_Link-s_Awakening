#include "stdafx.h"

#include "MoriblinSwordStaggerState.h"
#include "MoriblinSwordAttackState.h"

using namespace MoriblinSword;

CStaggerState::CStaggerState()
{
}

CMoriblinSwordState * CStaggerState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	return nullptr;
}

CMoriblinSwordState * CStaggerState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CStaggerState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CStaggerState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_STAGGER;

	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_STAGGER);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->PlaySounds(TEXT("Moriblin_Stagger.wav"), SOUND_MONSTER_EFFECT, .7f);
	RELEASE_INSTANCE(CGameInstance);
}

void CStaggerState::Exit(CMoriblinSword * pMoriblinSword)
{
}