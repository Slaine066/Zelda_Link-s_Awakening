#include "stdafx.h"

#include "BossblinAttackState.h"
#include "BossblinIdleState.h"
#include "Projectile.h"
#include "Collider.h"
#include "Weapon.h"

using namespace Bossblin;

CAttackState::CAttackState()
{
}

CBossblinState * CAttackState::AI_Behavior(CBossblin * pBossblin)
{
	Find_Target(pBossblin, true);

	return nullptr;
}

CBossblinState * CAttackState::Tick(CBossblin * pBossblin, _float fTimeDelta)
{
	pBossblin->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pBossblin->Is_AnimationLoop(pBossblin->Get_Model()->Get_CurrentAnimIndex()));
	pBossblin->Sync_WithNavigationHeight();

	/* Look At Target. */
	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pBossblin->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pBossblin->Get_Transform()->LookAt(vTargetPosition);

	/* Create Projectile on the 8th Keyframes (only IF there are no other Projectiles alive).  */
	if ((pBossblin->Get_Model()->Is_Keyframe("attach_R", 29)) == true && !pBossblin->Get_IsProjectileAlive())
	{
		Create_Projectile(pBossblin);
		pBossblin->Set_ShouldRenderWeapon(false);
		pBossblin->Set_IsProjectileAlive(true);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_tchar pSoundName[MAX_PATH] = TEXT("");
		_uint iRand = rand() % 4;
		wsprintf(pSoundName, TEXT("Spear_Swing_%d.wav"), iRand);

		pGameInstance->PlaySounds(pSoundName, SOUND_OBJECT, .5f);
		RELEASE_INSTANCE(CGameInstance);
	}

	return nullptr;
}

CBossblinState * CAttackState::LateTick(CBossblin * pBossblin, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState(m_pTarget);

	return nullptr;
}

void CAttackState::Enter(CBossblin * pBossblin)
{
	m_eStateId = STATE_ID::STATE_ATTACK_SPEAR;
	
	pBossblin->Set_ShouldRenderWeapon(true);
	pBossblin->Get_Model()->Set_CurrentAnimIndex(CBossblin::ANIMID::ANIM_SPEAR_THROW);
}

void CAttackState::Exit(CBossblin * pBossblin)
{
	pBossblin->Set_ShouldRenderWeapon(false);
}

void CAttackState::Create_Projectile(CBossblin * pBossblin)
{
	CProjectile::PROJECTILEDESC tProjectileDesc;
	ZeroMemory(&tProjectileDesc, sizeof(CProjectile::PROJECTILEDESC));
	tProjectileDesc.pOwner = pBossblin;
	tProjectileDesc.eProjectileType = CProjectile::PROJECTILE_TYPE::PROJECTILE_BOSSBLINSPEAR;
	tProjectileDesc.bIsPlayerProjectile = false;
	tProjectileDesc.fProjectileSpeed = 2.2f;
	tProjectileDesc.pModelPrototypeId = TEXT("Prototype_Component_Model_BossblinSpear");
	tProjectileDesc.vTargetPosition = m_pTarget->Get_Position();

	CGameObject* pGameObject = pBossblin->Get_Part(CBossblin::PARTS::PARTS_SPEAR);
	if (!pGameObject)
		return;
	CWeapon* pWeapon = dynamic_cast<CWeapon*>(pGameObject);
	if (!pWeapon)
		return;
	tProjectileDesc.mWorldMatrix = pWeapon->Get_CombinedWorldMatrix();

	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
	tColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_OUTPUT;
	tColliderDesc.vScale = _float3(.3f, .3f, 2.3f);
	tColliderDesc.vPosition = _float3(0.f, 0.f, .3f);
	tProjectileDesc.tColliderDesc = tColliderDesc;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("BossblinSpear_Projectile"), TEXT("Prototype_GameObject_Projectile"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Projectile"), &tProjectileDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}