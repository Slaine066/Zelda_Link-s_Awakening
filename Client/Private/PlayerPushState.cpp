#include "stdafx.h"

#include "PlayerPushState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"
#include "PlayerBareState.h"
#include "PlayerPushMoveState.h"
#include "Inventory.h"

using namespace Player;

CPushState::CPushState(STATETYPE eStateType, DIRID eDir, CActor* pPushedObject) 
	: m_eDirection(eDir), m_pPushedObject(pPushedObject)
{
	m_eStateType = eStateType;
}

CPlayerState * CPushState::HandleInput(CPlayer * pPlayer)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eStateType)
	{
	case STATETYPE_MAIN:
		if (pGameInstance->Key_Pressing(VK_LEFT) && m_eDirection == DIR_LEFT ||
			pGameInstance->Key_Pressing(VK_RIGHT) && m_eDirection == DIR_RIGHT ||
			pGameInstance->Key_Pressing(VK_DOWN) && m_eDirection == DIR_BACKWARD ||
			pGameInstance->Key_Pressing(VK_UP) && m_eDirection == DIR_STRAIGHT)
		{
			if (m_pPushedObject->Get_IsPushable())
				return new CPushMoveState(m_eDirection);
		}
		else
		{
			if (!CInventory::Get_Instance()->Get_Shield())
				return new CBareState();
			else
				return new CIdleState();
		}
			
		break;
	}

	return nullptr;
}

CPlayerState * CPushState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CPushState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
	{
		switch (m_eStateType)
		{
		case STATETYPE_START:
			return new CPushState(STATETYPE_MAIN, m_eDirection, m_pPushedObject);
			break;
		}
	}

	return nullptr;
}

void CPushState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_PUSH;

	switch (m_eStateType)
	{
	case STATETYPE_START:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_PUSH_START);
		break;
	case STATETYPE_MAIN:
		pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_PUSH_IDLE);
		break;
	}
}

void CPushState::Exit(CPlayer * pPlayer)
{

}