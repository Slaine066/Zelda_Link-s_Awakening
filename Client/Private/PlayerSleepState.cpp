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
	pPlayer->Get_Model()->Play_Animation(fTimeDelta * 1.5, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	if (!m_bIsSleeping)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		m_pBed = (CBed*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Object"));
		m_pBed->Set_Sleep();

		RELEASE_INSTANCE(CGameInstance);

		m_bIsSleeping = true;
	}

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