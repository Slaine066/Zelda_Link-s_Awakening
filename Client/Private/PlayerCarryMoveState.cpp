#include "stdafx.h"

#include "PlayerCarryMoveState.h"
#include "GameInstance.h"
#include "PlayerCarryState.h"
#include "PlayerThrowState.h"

using namespace Player;

CCarryMoveState::CCarryMoveState(DIRID eDir, _bool bFirstSmokeSpawned, _bool bSecondSmokeSpawned)
	: m_eDirection(eDir),
	m_bFirstSmokeSpawned(bFirstSmokeSpawned),
	m_bSecondSmokeSpawned(bSecondSmokeSpawned)
{

}

CPlayerState * CCarryMoveState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Down('A'))
		return new CThrowState();
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CCarryMoveState(DIR_STRAIGHT_LEFT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CCarryMoveState(DIR_STRAIGHT_RIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CCarryMoveState(DIR_BACKWARD_LEFT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CCarryMoveState(DIR_BACKWARD_RIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_LEFT))
		return new CCarryMoveState(DIR_LEFT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_RIGHT))
		return new CCarryMoveState(DIR_RIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_DOWN))
		return new CCarryMoveState(DIR_BACKWARD, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else if (pGameInstance->Key_Pressing(VK_UP))
		return new CCarryMoveState(DIR_STRAIGHT, m_bFirstSmokeSpawned, m_bSecondSmokeSpawned);
	else
		return new CCarryState(STATETYPE::STATETYPE_MAIN);

	return nullptr;
}

CPlayerState * CCarryMoveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	Move(pPlayer, fTimeDelta);

	pPlayer->Get_Model()->Play_Animation(fTimeDelta * 1.5f, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));

	return nullptr;
}

CPlayerState * CCarryMoveState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
		return nullptr;
}

void CCarryMoveState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_CARRY_MOVE;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_MOVE_CARRY);
	pPlayer->Get_Transform()->Change_Speed(pPlayer->Get_Stats().m_fWalkSpeed);
}

void CCarryMoveState::Exit(CPlayer * pPlayer)
{
	pPlayer->Get_Transform()->Change_Speed(pPlayer->Get_Stats().m_fRunSpeed);
}

void CCarryMoveState::Move(CPlayer * pPlayer, _float fTimeDelta)
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