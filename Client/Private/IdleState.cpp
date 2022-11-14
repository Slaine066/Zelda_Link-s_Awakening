#include "stdafx.h"

#include "IdleState.h"
#include "GameInstance.h"
#include "AttackState.h"
#include "MoveState.h"

CIdleState::CIdleState()
{
}

CPlayerState * CIdleState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
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
	else if (pGameInstance->Key_Down('S'))
		return new CAttackState();
	else
		return new CIdleState(); // When there are no Input.

	return nullptr;
}

CPlayerState * CIdleState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();
	
	return nullptr;
}

CPlayerState * CIdleState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	return nullptr;
}

void CIdleState::Enter(CPlayer * pPlayer)
{
	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_IDLE);
}

void CIdleState::Exit(CPlayer * pPlayer)
{

}
