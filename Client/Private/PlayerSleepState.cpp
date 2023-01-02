#include "stdafx.h"

#include "PlayerSleepState.h"
#include "GameInstance.h"
#include "PlayerWakeupState.h"

using namespace Player;

CSleepState::CSleepState()
{
}

CPlayerState * CSleepState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Up('A'))
		return new CWakeupState(STATETYPE_START);

	return nullptr;
}

CPlayerState * CSleepState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CSleepState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	return nullptr;
}

void CSleepState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_SLEEP;
	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SLEEP_LOOP);
}

void CSleepState::Exit(CPlayer * pPlayer)
{

}