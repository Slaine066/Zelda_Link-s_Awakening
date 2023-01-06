#include "stdafx.h"

#include "PlayerBareState.h"
#include "GameInstance.h"
#include "PlayerBareMoveState.h"
#include "PlayerAchieveState.h"
#include "Npc.h"

using namespace Player;

CBareState::CBareState()
{
}

CPlayerState * CBareState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

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

		m_pNpc = pPlayer->Get_Npc();
	}
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CBareMoveState(DIR_STRAIGHT_LEFT);
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CBareMoveState(DIR_STRAIGHT_RIGHT);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
		return new CBareMoveState(DIR_BACKWARD_LEFT);
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
		return new CBareMoveState(DIR_BACKWARD_RIGHT);
	else if (pGameInstance->Key_Pressing(VK_LEFT))
		return new CBareMoveState(DIR_LEFT);
	else if (pGameInstance->Key_Pressing(VK_RIGHT))
		return new CBareMoveState(DIR_RIGHT);
	else if (pGameInstance->Key_Pressing(VK_DOWN))
		return new CBareMoveState(DIR_BACKWARD);
	else if (pGameInstance->Key_Pressing(VK_UP))
		return new CBareMoveState(DIR_STRAIGHT);

	return nullptr;
}

CPlayerState * CBareState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CBareState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (!m_pNpc)
		return nullptr;

	if (m_pNpc)
	{
		m_pNpc->Interact();
		m_pNpc = nullptr;
	}

	return nullptr;
}

void CBareState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_BARE;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_IDLE_BARE);
}

void CBareState::Exit(CPlayer * pPlayer)
{

}
