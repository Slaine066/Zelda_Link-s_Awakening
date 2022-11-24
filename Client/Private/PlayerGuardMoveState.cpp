#include "stdafx.h"

#include "PlayerGuardMoveState.h"
#include "GameInstance.h"
#include "PlayerGuardState.h"
#include "PlayerAttackState.h"

using namespace Player;

CGuardMoveState::CGuardMoveState(DIRID eDir) : m_eDirection(eDir)
{
}

CPlayerState * CGuardMoveState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Up('W'))
		return new CGuardState(STATETYPE_END);
	else if (pGameInstance->Key_Up('S'))
		return new CAttackState();
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CGuardMoveState(DIR_STRAIGHT_LEFT);
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CGuardMoveState(DIR_STRAIGHT_RIGHT);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CGuardMoveState(DIR_BACKWARD_LEFT);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CGuardMoveState(DIR_BACKWARD_RIGHT);
	else if (pGameInstance->Key_Pressing(VK_LEFT))
		return new CGuardMoveState(DIR_LEFT);
	else if (pGameInstance->Key_Pressing(VK_RIGHT))
		return new CGuardMoveState(DIR_RIGHT);
	else if (pGameInstance->Key_Pressing(VK_DOWN))
		return new CGuardMoveState(DIR_BACKWARD);
	else if (pGameInstance->Key_Pressing(VK_UP))
		return new CGuardMoveState(DIR_STRAIGHT);
	else
		return new CGuardState(STATETYPE_MAIN);

	return nullptr;
}

CPlayerState * CGuardMoveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	Move(pPlayer, fTimeDelta);

	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CGuardMoveState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	return nullptr;
}

void CGuardMoveState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_GUARD_MOVE;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_HOLD_FRONT);
	pPlayer->Get_Transform()->Change_Speed(pPlayer->Get_Stats().m_fWalkSpeed);
}

void CGuardMoveState::Exit(CPlayer * pPlayer)
{
	pPlayer->Get_Transform()->Change_Speed(pPlayer->Get_Stats().m_fRunSpeed);
}

void CGuardMoveState::Move(CPlayer * pPlayer, _float fTimeDelta)
{
	switch (m_eDirection)
	{
	case DIR_STRAIGHT_LEFT:
		pPlayer->Get_Transform()->Set_RotationY(305.f);
		break;
	case DIR_STRAIGHT_RIGHT:
		pPlayer->Get_Transform()->Set_RotationY(45.f);
		break;
	case DIR_BACKWARD_LEFT:
		pPlayer->Get_Transform()->Set_RotationY(225.f);
		break;
	case DIR_BACKWARD_RIGHT:
		pPlayer->Get_Transform()->Set_RotationY(135.f);
		break;
	case DIR_STRAIGHT:
		pPlayer->Get_Transform()->Set_RotationY(0.f);
		break;
	case DIR_BACKWARD:
		pPlayer->Get_Transform()->Set_RotationY(180.f);
		break;
	case DIR_LEFT:
		pPlayer->Get_Transform()->Set_RotationY(270.f);
		break;
	case DIR_RIGHT:
		pPlayer->Get_Transform()->Set_RotationY(90.f);
		break;
	}

	pPlayer->Get_Transform()->Move_Straight(fTimeDelta, pPlayer->Get_Navigation(), pPlayer->Get_Radius());
}