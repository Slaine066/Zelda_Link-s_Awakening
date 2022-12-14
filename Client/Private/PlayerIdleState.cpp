#include "stdafx.h"

#include "PlayerIdleState.h"
#include "GameInstance.h"
#include "PlayerAttackState.h"
#include "PlayerMoveState.h"
#include "PlayerGuardState.h"
#include "PlayerJumpState.h"
#include "PlayerAchieveState.h"
#include "InteractableObject.h"
#include "Npc.h"
#include "Inventory.h"
#include "Monster.h"

using namespace Player;

CIdleState::CIdleState()
{
}

CPlayerState * CIdleState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pPlayer->Get_Monster())
		if (pPlayer->Get_Monster()->Get_CurrentChat())
		{
			if (pGameInstance->Key_Down('A'))
				m_pMonster = pPlayer->Get_Monster();

			return nullptr;
		}

	if (pPlayer->Get_Npc())
		if (pPlayer->Get_Npc()->Get_CurrentChat())
		{
			if (pGameInstance->Key_Down('A'))
				m_pNpc = pPlayer->Get_Npc();

			return nullptr;
		}

	if (pGameInstance->Key_Down('A'))
	{
		if (pPlayer->Get_Npc() && !pPlayer->Get_Npc()->Get_DidInteract())
			m_pNpc = pPlayer->Get_Npc();

		m_pMonster = pPlayer->Get_Monster();

		if (pPlayer->Get_InteractableObject() && !pPlayer->Get_InteractableObject()->Get_DidInteract())
			m_pInteractableObject = pPlayer->Get_InteractableObject();
	}
	else if (pGameInstance->Key_Down('S'))
	{
		if (CInventory::Get_Instance()->Get_Sword())
			return new CAttackState();
	}
	else if (pGameInstance->Key_Down('W'))
	{
		if (CInventory::Get_Instance()->Get_Shield())
			return new CGuardState(STATETYPE_START);
	}
	else if (pGameInstance->Key_Down('Z'))
		return pPlayer->Use_Item();
	else if (pGameInstance->Key_Down('X'))
		return pPlayer->Use_Item(false);
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
	if (!m_pInteractableObject && !m_pMonster && !m_pNpc)
		return nullptr;

	if (m_pNpc)
	{
		m_pNpc->Interact();
		m_pNpc = nullptr;
	}
	else if (m_pMonster)
	{
		m_pMonster->Show_Chat();
		m_pMonster = nullptr;
	}
	else if (m_pInteractableObject)
	{
		m_pInteractableObject->Interact();
		m_pInteractableObject = nullptr;
	}
	
	return nullptr;
}

void CIdleState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_IDLE;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_IDLE);
}

void CIdleState::Exit(CPlayer * pPlayer)
{

}
