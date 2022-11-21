#include "stdafx.h"

#include "MoriblinSwordAttackState.h"
#include "MoriblinSwordIdleState.h"
#include "MoriblinSwordStaggerState.h"
#include "PlayerState.h"
#include "PlayerGuardState.h"

using namespace MoriblinSword;

CAttackState::CAttackState(_bool bIsHit) : m_bIsHit(bIsHit)
{
}

CMoriblinSwordState * CAttackState::AI_Behavior(CMoriblinSword * pMoriblinSword)
{
	Find_Target(pMoriblinSword, true);

	return nullptr;
}

CMoriblinSwordState * CAttackState::Tick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (m_bIsHit)
		BounceBack(pMoriblinSword, fTimeDelta);
	else
		Move(pMoriblinSword, fTimeDelta);

	if (m_bIsHit && m_fPushBackTimer < 0.6f)
		m_fPushBackTimer += fTimeDelta;
	else
		m_bIsHit = false;

	pMoriblinSword->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSword->Is_AnimationLoop(pMoriblinSword->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSword->Sync_WithNavigationHeight();

	return nullptr;
}

CMoriblinSwordState * CAttackState::LateTick(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (!m_bDidDamage)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		vector<CGameObject*> pDamagedObjects;
		pGameInstance->Collision_Check_Group_Multi(CCollision_Manager::COLLISION_GROUP::COLLISION_PLAYER, pMoriblinSword->Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
		RELEASE_INSTANCE(CGameInstance);

		if (!pDamagedObjects.empty())
		{
			for (auto& pDamaged : pDamagedObjects)
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pDamaged);
				if (!pPlayer)
					continue;

				if (pPlayer->Get_State()->Get_StateId() == CPlayerState::STATE_ID::STATE_GUARD ||
					pPlayer->Get_State()->Get_StateId() == CPlayerState::STATE_ID::STATE_GUARD_MOVE)
				{
					pPlayer->Get_Model()->Reset_CurrentAnimation();
					CPlayerState* pGuardState = new CGuardState(CPlayerState::STATETYPE::STATETYPE_START);
					pPlayer->Set_State(pPlayer->Get_State()->ChangeState(pPlayer, pPlayer->Get_State(), pGuardState));

					pMoriblinSword->Get_Model()->Reset_CurrentAnimation();
					return new CStaggerState();
				}
				else
					pPlayer->Take_Damage(pMoriblinSword->Get_Stats().m_fAttackPower, nullptr, pMoriblinSword);
			}
			m_bDidDamage = true;
		}
	}

	if (m_bIsAnimationFinished && m_bDidDamage)
		return new CIdleState(m_pTarget);

	return nullptr;
}

void CAttackState::Enter(CMoriblinSword * pMoriblinSword)
{
	m_eStateId = STATE_ID::STATE_ATTACK;

	pMoriblinSword->Get_Model()->Set_CurrentAnimIndex(CMoriblinSword::ANIMID::ANIM_STANCE_WALK);
}

void CAttackState::Exit(CMoriblinSword * pMoriblinSword)
{
	m_bDidDamage = false;
}

void CAttackState::Move(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	if (!m_pTarget)
		return;

	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSword->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pMoriblinSword->Get_Transform()->LookAt(vTargetPosition);

	_float3 vPosition;
	XMStoreFloat3(&vPosition, vTargetPosition);

	_float fAttackRadius = pMoriblinSword->Get_AttackRadius();
	pMoriblinSword->Get_Transform()->Go_TargetPosition(fTimeDelta, vPosition, fAttackRadius, pMoriblinSword->Get_Navigation());
}

void CAttackState::BounceBack(CMoriblinSword * pMoriblinSword, _float fTimeDelta)
{
	_vector vTargetPosition = m_pTarget->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vPosition = pMoriblinSword->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector BounceDir = vPosition - vTargetPosition;
	BounceDir = XMVector4Normalize(BounceDir);

	pMoriblinSword->Get_Transform()->Move_Direction(BounceDir, fTimeDelta, pMoriblinSword->Get_Navigation());
}