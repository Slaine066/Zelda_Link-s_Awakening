#include "stdafx.h"

#include "TarinTalkState.h"
#include "TarinIdleState.h"
#include "UI.h"

using namespace Tarin;

CTalkState::CTalkState()
{
}

CTarinState * CTalkState::AI_Behavior(CTarin * pTarin)
{
	return nullptr;
}

CTarinState * CTalkState::Tick(CTarin * pTarin, _float fTimeDelta)
{
	pTarin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pTarin->Is_AnimationLoop(pTarin->Get_Model()->Get_CurrentAnimIndex()));
	pTarin->Sync_WithNavigationHeight();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_fSoundTimer > 3.f)
	{
		_tchar pSoundName[MAX_PATH] = TEXT("");
		wsprintf(pSoundName, TEXT("Tarin_Talk_%d.wav"), m_iSoundCounter);

		pGameInstance->PlaySounds(pSoundName, SOUND_NPC, 1.f);

		m_iSoundCounter++;
		m_fSoundTimer = 0.f;
	}
	else
		m_fSoundTimer += fTimeDelta;

	RELEASE_INSTANCE(CGameInstance);

	return nullptr;
}

CTarinState * CTalkState::LateTick(CTarin * pTarin, _float fTimeDelta)
{
	if (!pTarin->Get_CurrentChat())
		return new CIdleState();

	pTarin->CanInteract();

	return nullptr;
}

void CTalkState::Enter(CTarin * pTarin)
{
	m_eStateId = STATE_ID::STATE_TALK;

	pTarin->Get_Model()->Set_CurrentAnimIndex(CTarin::ANIMID::ANIM_TALK);
}

void CTalkState::Exit(CTarin * pTarin)
{
}