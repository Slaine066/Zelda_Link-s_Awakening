#include "stdafx.h"

#include "PlayerCarryState.h"
#include "GameInstance.h"
#include "PlayerCarryMoveState.h"
#include "PlayerThrowState.h"

using namespace Player;

CCarryState::CCarryState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CPlayerState * CCarryState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eStateType)
	{
		case STATETYPE::STATETYPE_MAIN:
		{
			if (pGameInstance->Key_Down('A'))
				return new CThrowState();
			else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
				return new CCarryMoveState(DIR_STRAIGHT_LEFT);
			else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
				return new CCarryMoveState(DIR_STRAIGHT_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
				return new CCarryMoveState(DIR_BACKWARD_LEFT);
			else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
				return new CCarryMoveState(DIR_BACKWARD_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_LEFT))
				return new CCarryMoveState(DIR_LEFT);
			else if (pGameInstance->Key_Pressing(VK_RIGHT))
				return new CCarryMoveState(DIR_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_DOWN))
				return new CCarryMoveState(DIR_BACKWARD);
			else if (pGameInstance->Key_Pressing(VK_UP))
				return new CCarryMoveState(DIR_STRAIGHT);

			break;
		}
	}

	return nullptr;
}

CPlayerState * CCarryState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));

	return nullptr;
}

CPlayerState * CCarryState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
			case STATETYPE_START:
				return new CCarryState(STATETYPE_MAIN);
		}
	}

	return nullptr;
}

void CCarryState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_CARRY;

	switch (m_eStateType)
	{
		case STATETYPE_START:
			pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_CARRY);
			pPlayer->Set_WeaponBomb();

			PlaySound(pPlayer);
			break;
		case STATETYPE_MAIN:
			pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_IDLE_CARRY);
			break;
	}
}

void CCarryState::Exit(CPlayer * pPlayer)
{
}

void CCarryState::PlaySound(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_tchar pSoundName[MAX_PATH] = TEXT("");
	_uint iRand = rand() % 4;
	wsprintf(pSoundName, TEXT("Link_Carry_%d.wav"), iRand);

	pGameInstance->PlaySounds(pSoundName, SOUND_PLAYER, 1.f);

	RELEASE_INSTANCE(CGameInstance);
}