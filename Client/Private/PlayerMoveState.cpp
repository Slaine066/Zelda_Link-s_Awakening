#include "stdafx.h"

#include "PlayerMoveState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"
#include "PlayerAttackState.h"

CMoveState::CMoveState(DIRID eDir) : m_eDirection(eDir) 
{
}

CPlayerState * CMoveState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Down('S'))
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
	else 
		return new CIdleState();

	return nullptr;
}

CPlayerState * CMoveState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	Move(pPlayer, fTimeDelta);

	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CMoveState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	return nullptr;
}

void CMoveState::Enter(CPlayer * pPlayer)
{
	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_RUN);
}

void CMoveState::Exit(CPlayer * pPlayer)
{

}

void CMoveState::Move(CPlayer * pPlayer, _float fTimeDelta)
{
	switch (m_eDirection)
	{
	case DIR_STRAIGHT_LEFT:
		pPlayer->Get_Transform()->Go_Straight_Left(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_STRAIGHT_RIGHT:
		pPlayer->Get_Transform()->Go_Straight_Right(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_BACKWARD_LEFT:
		pPlayer->Get_Transform()->Go_Backward_Left(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_BACKWARD_RIGHT:
		pPlayer->Get_Transform()->Go_Backward_Right(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_STRAIGHT:
		pPlayer->Get_Transform()->Go_Straight(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_BACKWARD:
		pPlayer->Get_Transform()->Go_Backward(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_LEFT:
		pPlayer->Get_Transform()->Go_Left(fTimeDelta, pPlayer->Get_Navigation());
		break;
	case DIR_RIGHT:
		pPlayer->Get_Transform()->Go_Right(fTimeDelta, pPlayer->Get_Navigation());
		break;
	}
}