#include "stdafx.h"

#include "PlayerGuardState.h"
#include "GameInstance.h"
#include "PlayerAttackState.h"
#include "PlayerMoveState.h"
#include "PlayerIdleState.h"
#include "PlayerGuardMoveState.h"

CGuardState::CGuardState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CPlayerState * CGuardState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eStateType)
	{
		case STATE_MAIN:
			if (pGameInstance->Key_Up('W'))
				return new CGuardState(STATE_END);
			else if (pGameInstance->Key_Pressing(VK_LEFT))
				return new CGuardMoveState(DIR_LEFT);
			else if (pGameInstance->Key_Pressing(VK_RIGHT))
				return new CGuardMoveState(DIR_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_DOWN))
				return new CGuardMoveState(DIR_BACKWARD);
			else if (pGameInstance->Key_Pressing(VK_UP))
				return new CGuardMoveState(DIR_STRAIGHT);
			break;
		case STATE_END:
			if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
				return new CMoveState(DIR_STRAIGHT_LEFT);
			else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
				return new CMoveState(DIR_STRAIGHT_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
				return new CMoveState(DIR_BACKWARD_LEFT);
			else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
				return new CMoveState(DIR_BACKWARD_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_LEFT))
				return new CMoveState(DIR_LEFT);
			else if (pGameInstance->Key_Pressing(VK_RIGHT))
				return new CMoveState(DIR_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_DOWN))
				return new CMoveState(DIR_BACKWARD);
			else if (pGameInstance->Key_Pressing(VK_UP))
				return new CMoveState(DIR_STRAIGHT);
			break;
	}

	return nullptr;
}

CPlayerState * CGuardState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CGuardState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished) 
	{
		switch (m_eStateType)
		{
		case STATE_START:
			return new CGuardState(STATE_MAIN);
			break;
		case STATE_END:
			return new CIdleState();
			break;
		}
	}

	return nullptr;
}

void CGuardState::Enter(CPlayer * pPlayer)
{
	switch (m_eStateType)
	{
	case STATE_START:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_START);
		break;
	case STATE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_LOOP);
		break;
	case STATE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_END);
		break;
	}
}

void CGuardState::Exit(CPlayer * pPlayer)
{

}