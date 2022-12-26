#include "stdafx.h"

#include "PlayerGuardState.h"
#include "GameInstance.h"
#include "PlayerAttackState.h"
#include "PlayerMoveState.h"
#include "PlayerIdleState.h"
#include "PlayerGuardMoveState.h"
#include "Effect.h"

using namespace Player;

CGuardState::CGuardState(STATETYPE eStateType)
{
	m_eStateType = eStateType;
}

CPlayerState * CGuardState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eStateType)
	{
		case STATETYPE_MAIN:
			if (pGameInstance->Key_Up('W'))
				return new CGuardState(STATETYPE_END);
			else if (pGameInstance->Key_Up('S'))
				return new CAttackState();
			else if (pGameInstance->Key_Pressing(VK_LEFT))
				return new CGuardMoveState(DIR_LEFT);
			else if (pGameInstance->Key_Pressing(VK_RIGHT))
				return new CGuardMoveState(DIR_RIGHT);
			else if (pGameInstance->Key_Pressing(VK_DOWN))
				return new CGuardMoveState(DIR_BACKWARD);
			else if (pGameInstance->Key_Pressing(VK_UP))
				return new CGuardMoveState(DIR_STRAIGHT);
			break;
		case STATETYPE_END:
			if (pGameInstance->Key_Up('S'))
				return new CAttackState();
			else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
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
		case STATETYPE_START:
			return new CGuardState(STATETYPE_MAIN);
			break;
		case STATETYPE_END:
			return new CIdleState();
			break;
		}
	}

	return nullptr;
}

void CGuardState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_GUARD;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_START);
		break;
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_LOOP);
		break;
	case STATETYPE_END:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_END);
		break;
	}
}

void CGuardState::Exit(CPlayer * pPlayer)
{

}