#include "stdafx.h"

#include "PlayerWakeupState.h"
#include "GameInstance.h"
#include "PlayerBareState.h"

using namespace Player;

CWakeupState::CWakeupState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CPlayerState * CWakeupState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eStateType)
	{
		case STATETYPE_MAIN:
		{
			if (pGameInstance->Key_Down('A'))
				return new CWakeupState(STATETYPE_END);

			break;
		}
	}

	return nullptr;
}

CPlayerState * CWakeupState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta * 1.5f, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	if (m_eStateType == STATETYPE::STATETYPE_END && !m_bSoundPlayed)
	{
		if (pPlayer->Get_Model()->Is_Keyframe("spine_a", 55))
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			pGameInstance->PlaySounds(TEXT("Link_OutOfBed.wav"), SOUND_PLAYER, 1.f);
			RELEASE_INSTANCE(CGameInstance);

			m_bSoundPlayed = true;
		}
	}

	return nullptr;
}

CPlayerState * CWakeupState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CWakeupState(STATETYPE_MAIN);
		case STATETYPE_END:
			return new CBareState();
		}
	}

	return nullptr;
}

void CWakeupState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_WAKEUP;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pBed = (CBed*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Object"));

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_WAKEUP_START);
		m_pBed->Set_Wakeup();

		pGameInstance->PlaySounds(TEXT("Link_Wakeup.wav"), SOUND_PLAYER, 1.f);
		break;
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_WAKEUP_LOOP);
		
		pGameInstance->PlayBGM(TEXT("Marin_House.mp3"), 0.5f);
		pGameInstance->Set_CurrentBGM(TEXT("Marin_House.mp3"));
		break;
	case STATETYPE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_WAKEUP_END);
		m_pBed->Set_Getup();
		break;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CWakeupState::Exit(CPlayer * pPlayer)
{
	switch (m_eStateType)
	{
		case STATETYPE_END:
			pPlayer->Set_SleepOffset(true);
			break;
	}
}