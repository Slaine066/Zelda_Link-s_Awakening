#include "stdafx.h"

#include "PlayerPushMoveState.h"
#include "GameInstance.h"

using namespace Player;

CPushMoveState::CPushMoveState(DIRID eDir) : m_eDirection(eDir)
{
}

CPlayerState * CPushMoveState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CPushMoveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	Move(pPlayer, fTimeDelta);

	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CPushMoveState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	return nullptr;
}

void CPushMoveState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_GUARD_MOVE;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SHIELD_HOLD_FRONT);
	pPlayer->Get_Transform()->Change_Speed(pPlayer->Get_Stats().m_fWalkSpeed);
}

void CPushMoveState::Exit(CPlayer * pPlayer)
{
	pPlayer->Get_Transform()->Change_Speed(pPlayer->Get_Stats().m_fRunSpeed);
}

void CPushMoveState::Move(CPlayer * pPlayer, _float fTimeDelta)
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