#include "stdafx.h"

#include "PlayerThrowState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"

using namespace Player;

CThrowState::CThrowState()
{
}

CPlayerState * CThrowState::HandleInput(CPlayer * pPlayer)
{
	return nullptr;
}

CPlayerState * CThrowState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	if (pPlayer->Get_Model()->Is_Keyframe("itemA_L", 12) && !m_bIsBombDisappeared)
	{
		/* Be sure to call this function before "Unset_WeaponBomb()" because it needs the Weapon position in order spawn. */
		pPlayer->Spawn_BombProjectile();
		pPlayer->Unset_WeaponBomb();
	
		m_bIsBombDisappeared = true;
	}

	return nullptr;
}

CPlayerState * CThrowState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState();

	return nullptr;
}

void CThrowState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_THROW;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_THROW);
}

void CThrowState::Exit(CPlayer * pPlayer)
{
	pPlayer->Set_ShowSword(true);
}