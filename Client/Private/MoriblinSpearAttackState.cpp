#include "stdafx.h"

#include "MoriblinSpearAttackState.h"
#include "MoriblinSpearIdleState.h"
#include "Projectile.h"
#include "Collider.h"
#include "Weapon.h"

using namespace MoriblinSpear;

CAttackState::CAttackState()
{
}

CMoriblinSpearState * CAttackState::AI_Behavior(CMoriblinSpear * pMoriblinSpear)
{
	Find_Target(pMoriblinSpear, true);

	return nullptr;
}

CMoriblinSpearState * CAttackState::Tick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	pMoriblinSpear->Get_Model()->Play_Animation(fTimeDelta, m_bIsAnimationFinished, pMoriblinSpear->Is_AnimationLoop(pMoriblinSpear->Get_Model()->Get_CurrentAnimIndex()));
	pMoriblinSpear->Sync_WithNavigationHeight();

	/* Look At Target. */
	_vector vTargetPosition = XMVectorSet(m_pTarget->Get_Position().x, pMoriblinSpear->Get_Position().y, m_pTarget->Get_Position().z, 1.f);
	pMoriblinSpear->Get_Transform()->LookAt(vTargetPosition);

	/* Create Projectile on the 8th Keyframes (only IF there are no other Projectiles alive).  */
	if ((pMoriblinSpear->Get_Model()->Is_Keyframe("hand_L", 8)) == true && !pMoriblinSpear->Get_IsProjectileAlive())
	{
		Create_Projectile(pMoriblinSpear);
		pMoriblinSpear->Set_IsProjectileAlive(true);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_tchar pSoundName[MAX_PATH] = TEXT("");
		_uint iRand = rand() % 4;
		wsprintf(pSoundName, TEXT("Spear_Swing_%d.wav"), iRand);

		pGameInstance->PlaySounds(pSoundName, SOUND_OBJECT, .5f);
		RELEASE_INSTANCE(CGameInstance);
	}

	return nullptr;
}

CMoriblinSpearState * CAttackState::LateTick(CMoriblinSpear * pMoriblinSpear, _float fTimeDelta)
{
	if (m_bIsAnimationFinished)
		return new CIdleState(m_pTarget);

	return nullptr;
}

void CAttackState::Enter(CMoriblinSpear * pMoriblinSpear)
{
	m_eStateId = STATE_ID::STATE_ATTACK;

	pMoriblinSpear->Get_Model()->Set_CurrentAnimIndex(CMoriblinSpear::ANIMID::ANIM_SPEAR_THROW);
}

void CAttackState::Exit(CMoriblinSpear * pMoriblinSpear)
{
}

void CAttackState::Create_Projectile(CMoriblinSpear * pMoriblinSpear)
{
	CProjectile::PROJECTILEDESC tProjectileDesc;
	ZeroMemory(&tProjectileDesc, sizeof(CProjectile::PROJECTILEDESC));
	tProjectileDesc.pOwner = pMoriblinSpear;
	tProjectileDesc.eProjectileType = CProjectile::PROJECTILE_TYPE::PROJECTILE_MORIBLINSPEAR;
	tProjectileDesc.bIsPlayerProjectile = false;
	tProjectileDesc.fProjectileSpeed = 2.2f;
	tProjectileDesc.pModelPrototypeId = TEXT("Prototype_Component_Model_Spear");
	tProjectileDesc.vTargetPosition = m_pTarget->Get_Position();

	CGameObject* pGameObject = pMoriblinSpear->Get_Part(CMoriblinSpear::PARTS::PARTS_SPEAR);
	if (!pGameObject)
		return;
	CWeapon* pWeapon = dynamic_cast<CWeapon*>(pGameObject);
	if (!pWeapon)
		return;
	tProjectileDesc.mWorldMatrix = pWeapon->Get_CombinedWorldMatrix();

	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
	tColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_OUTPUT;
	tColliderDesc.vScale = _float3(.3f, .2f, 1.5f);
	tColliderDesc.vPosition = _float3(0.f, 0.f, .25f);
	tProjectileDesc.tColliderDesc = tColliderDesc;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Spear_Projectile"), TEXT("Prototype_GameObject_Projectile"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Projectile"), &tProjectileDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}