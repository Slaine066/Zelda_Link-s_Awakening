#include "stdafx.h"

#include "MarinTalkState.h"
#include "MarinIdleState.h"

using namespace Marin;

CTalkState::CTalkState()
{
}

CMarinState * CTalkState::AI_Behavior(CMarin * pMarin)
{
	return nullptr;
}

CMarinState * CTalkState::Tick(CMarin * pMarin, _float fTimeDelta)
{
	pMarin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMarin->Is_AnimationLoop(pMarin->Get_Model()->Get_CurrentAnimIndex()));
	pMarin->Sync_WithNavigationHeight();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_fSoundTimer > 3.f)
	{
		_tchar pSoundName[MAX_PATH] = TEXT("");
		wsprintf(pSoundName, TEXT("Marin_Talk_%d.wav"), m_iSoundCounter);

		pGameInstance->PlaySounds(pSoundName, SOUND_NPC, 1.f);

		m_iSoundCounter++;
		m_fSoundTimer = 0.f;
	}
	else
		m_fSoundTimer += fTimeDelta;

	return nullptr;
}

CMarinState * CTalkState::LateTick(CMarin * pMarin, _float fTimeDelta)
{
	if (!pMarin->Get_CurrentChat())
		return new CIdleState();

	pMarin->CanInteract();

	return nullptr;
}

void CTalkState::Enter(CMarin * pMarin)
{
	m_eStateId = STATE_ID::STATE_TALK;

	pMarin->Get_Model()->Set_CurrentAnimIndex(CMarin::ANIMID::ANIM_TALK);
}

void CTalkState::Exit(CMarin * pMarin)
{
}