#include "stdafx.h"

#include "PlayerAttackState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"

CAttackState::CAttackState()
{
}

CPlayerState * CAttackState::HandleInput(CPlayer * pPlayer)
{
	if ((pPlayer->Get_Model()->Over_Keyframe("hand_L", 9)) == true)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (pGameInstance->Key_Down('S'))
		{
			pPlayer->Get_Model()->Reset_CurrentAnimation();
			return new CAttackState();
		}
	}

	return nullptr;
}

CPlayerState * CAttackState::Tick(CPlayer * pPlayer, _float fTimeDelta)
{
	pPlayer->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
	pPlayer->Sync_WithNavigationHeight();

	return nullptr;
}

CPlayerState * CAttackState::LateTick(CPlayer * pPlayer, _float fTimeDelta)
{
	if (!m_bDidDamage)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		vector<CGameObject*> pDamagedObjects;
		pGameInstance->Collision_Check_Group_Multi(CCollision_Manager::COLLISION_GROUP::COLLISION_MONSTER, pPlayer->Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
		RELEASE_INSTANCE(CGameInstance);

		if (!pDamagedObjects.empty())
		{
			for (auto& pDamaged : pDamagedObjects)
				pDamaged->Take_Damage(pPlayer->Get_Stats().m_fAttackPower, nullptr, pPlayer);

			m_bDidDamage = true;
		}
	}

	if (m_bIsAnimationFinished)
		return new CIdleState();

	return nullptr;
}

void CAttackState::Enter(CPlayer * pPlayer)
{
	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SLASH);
}

void CAttackState::Exit(CPlayer * pPlayer)
{
	m_bDidDamage = false;
}