#include "stdafx.h"

#include "PlayerAttackState.h"
#include "GameInstance.h"
#include "PlayerIdleState.h"
#include "Effect.h"

using namespace Player;

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
	pPlayer->Get_Model()->Play_Animation(fTimeDelta * 1.25, m_bIsAnimationFinished, pPlayer->Is_AnimationLoop(pPlayer->Get_Model()->Get_CurrentAnimIndex()));
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
			{
				/* Apply Damage */
				_float fDamage = pDamaged->Take_Damage(pPlayer->Get_Stats().m_fAttackPower, nullptr, pPlayer);

				/* Spawn Hit Effect */
				if (fDamage > 0.f)
					Spawn_HitEffect(pPlayer, pDamaged);
			}

			m_bDidDamage = true;
		}
	}

	if (m_bIsAnimationFinished)
		return new CIdleState();

	return nullptr;
}

void CAttackState::Enter(CPlayer * pPlayer)
{
	m_eStateId = STATE_ID::STATE_ATTACK;

	pPlayer->Get_Model()->Set_CurrentAnimIndex(CPlayer::ANIMID::ANIM_SLASH);
}

void CAttackState::Exit(CPlayer * pPlayer)
{
	m_bDidDamage = false;
}

void CAttackState::Spawn_HitEffect(CPlayer* pPlayer, CGameObject*& pDamaged)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_HIT_FLASH;
	tEffectDesc.m_fEffectLifespan = .25f;
	tEffectDesc.m_pOwner = pPlayer;

	CHierarchyNode* m_pSocket = pPlayer->Get_Model()->Get_BonePtr("itemA_L_top");
	_matrix SocketMatrix = m_pSocket->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&pPlayer->Get_Model()->Get_PivotFloat4x4()) * XMLoadFloat4x4(&pPlayer->Get_Transform()->Get_World4x4());
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrix);

	/* Spawn Hit Flash Effect (Rect Buffer) on Sword Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Hit_Flash_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_HIT_RING;
	
	/* Spawn Hit Ring Effect (Model) on Sword Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Hit_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_HIT;

	/* Spawn Hit Flash Effect (Model) on Sword Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Hit_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}