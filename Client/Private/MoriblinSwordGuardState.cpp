#include "stdafx.h"

#include "MoriblinSwordGuardState.h"
#include "MoriblinSwordAttackState.h"
#include "Transform.h"

using namespace MoriblinSword;

CGuardState::CGuardState()
{
}

CMoriblinSwordState * CGuardState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword, true);

	return nullptr;
}

CMoriblinSwordState * CGuardState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta * 2.5f, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	BounceBack(pMoriblinSword, fTimeDelta);

	if (pMoriblinSword->Get_Model()->Is_Keyframe("attach_R", 0) && !m_bEffectSpawned)
	{
		pMoriblinSword->Spawn_GuardEffect();
		m_bEffectSpawned = true;
	}

	return nullptr;
}

CMoriblinSwordState * CGuardState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CAttackState();

	return nullptr;
}

void CGuardState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_GUARD;

	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_GUARD);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar pSoundName[MAX_PATH] = TEXT("");
	_uint iRand = rand() % 4;
	wsprintf(pSoundName, TEXT("Guard_%d.wav"), iRand);

	pGameInstance->PlaySounds(pSoundName, SOUND_OBJECT, .5f);

	RELEASE_INSTANCE(CGameInstance);
}

void CGuardState::Exit(CMoriblinSword * pMoriblinSword)
{

}

void CGuardState::BounceBack(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pMoriblinSword->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pMoriblinSword->Get_Navigation(), pMoriblinSword->Get_Radius());
}
