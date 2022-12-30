#include "stdafx.h"

#include "Projectile.h"
#include "GameInstance.h"
#include "MoriblinSpear.h"
#include "Bossblin.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerGuardState.h"
#include "Effect.h"

using namespace Player;

CProjectile::CProjectile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile & rhs)
	: CActor(rhs)
{
}

HRESULT CProjectile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	switch (m_tProjectileDesc.eProjectileType)
	{
		case PROJECTILE_TYPE::PROJECTILE_MORIBLINSPEAR:
		case PROJECTILE_TYPE::PROJECTILE_BOSSBLINSPEAR:
		{
			_float4 vPosition;
			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

			_vector vTargetPosition = XMVectorSet(m_tProjectileDesc.vTargetPosition.x, vPosition.y, m_tProjectileDesc.vTargetPosition.z, 1.f);

			_vector vLook = vTargetPosition - XMLoadFloat4(&vPosition);
			m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, XMVector3Normalize(vLook) * m_pTransformCom->Get_Scale(CTransform::STATE::STATE_LOOK));

			break;
		}
		case PROJECTILE_TYPE::PROJECTILE_PLAYERBOMB:
		{
			_float4 vPosition;
			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

			_float4 vOwnerPosition;
			XMStoreFloat4(&vOwnerPosition, m_tProjectileDesc.pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION));
			vOwnerPosition.y = vPosition.y;

			_vector vDirection = XMLoadFloat4(&vPosition) - XMLoadFloat4(&vOwnerPosition);
			vDirection = XMVector4Normalize(vDirection);

			XMStoreFloat4(&m_vProjectileDirection, vDirection);

			break;
		}
	}

	return S_OK;
}

_uint CProjectile::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(m_tProjectileDesc.bIsPlayerProjectile ? CCollision_Manager::COLLISION_PLAYER : CCollision_Manager::COLLISION_MONSTER, this);

	switch (m_tProjectileDesc.eProjectileType)
	{
		case PROJECTILE_MORIBLINSPEAR:
			MoriblinSpear_Tick(fTimeDelta);
			break;
		case PROJECTILE_BOSSBLINSPEAR:
			BossblinSpear_Tick(fTimeDelta);
			break;
		case PROJECTILE_PLAYERBOMB:
			PlayerBomb_Tick(fTimeDelta);
			break;
	}

	return OBJ_NOEVENT;
}

_uint CProjectile::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	switch (m_tProjectileDesc.eProjectileType)
	{
		case PROJECTILE_MORIBLINSPEAR:
			MoriblinSpear_Collision();
			break;
		case PROJECTILE_BOSSBLINSPEAR:
			BossblinSpear_Collision();
			break;
		case PROJECTILE_PLAYERBOMB:
			PlayerBomb_Collision();
	}

	return OBJ_NOEVENT;
}

HRESULT CProjectile::Render()
{
	__super::Render();

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->SetUp_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderPass)))
			return E_FAIL;
	}

	// Only in Debug
	Render_Colliders();

	return S_OK;
}

HRESULT CProjectile::Ready_Components(void* pArg)
{
	if (pArg)
	{
		ZeroMemory(&m_tProjectileDesc, sizeof(PROJECTILEDESC));
		memcpy(&m_tProjectileDesc, pArg, sizeof(PROJECTILEDESC));
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tProjectileDesc.mWorldMatrix;
	TransformDesc.fSpeedPerSec = m_tProjectileDesc.fProjectileSpeed;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, m_tProjectileDesc.pModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	memcpy(&ColliderDesc, &m_tProjectileDesc.tColliderDesc, sizeof(CCollider::COLLIDERDESC));

	if (m_tProjectileDesc.eProjectileType == PROJECTILE_TYPE::PROJECTILE_PLAYERBOMB)
	{
		/* For.Com_Collider*/
		if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_Collider*/
		if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
			return E_FAIL;
	}

	CNavigation::NAVDESC NavDesc;
	ZeroMemory(&NavDesc, sizeof(CNavigation::NAVDESC));
	XMStoreFloat3(&NavDesc.vInitialPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	_uint iLevelIndex = CGameInstance::Get_Instance()->Get_NextLevelIndex();

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), iLevelIndex, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NavDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::SetUp_ShaderResources()
{
	if (!m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectTimer", &m_fCountdownTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectLifespan", &m_fCountdownLifetime, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CProjectile::MoriblinSpear_Collision()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<CGameObject*> pDamagedObjects;
	pGameInstance->Collision_Check_Group_Multi(m_tProjectileDesc.bIsPlayerProjectile ? CCollision_Manager::COLLISION_MONSTER : CCollision_Manager::COLLISION_PLAYER,
		Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
	RELEASE_INSTANCE(CGameInstance);

	if (!pDamagedObjects.empty())
	{
		CMoriblinSpear* pMoriblinSpear = dynamic_cast<CMoriblinSpear*>(m_tProjectileDesc.pOwner);
		if (!pMoriblinSpear)
			return;

		for (auto& pDamaged : pDamagedObjects)
		{
			if (m_tProjectileDesc.bIsPlayerProjectile)
				pDamaged->Take_Damage(pMoriblinSpear->Get_Stats().m_fAttackPower, nullptr, m_tProjectileDesc.pOwner);
			else
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pDamaged);
				if (!pPlayer)
					continue;

				if (pPlayer->Get_State()->Get_StateId() == CPlayerState::STATE_ID::STATE_GUARD ||
					pPlayer->Get_State()->Get_StateId() == CPlayerState::STATE_ID::STATE_GUARD_MOVE)
				{
					pPlayer->Get_Model()->Reset_CurrentAnimation();
					CPlayerState* pGuardState = new Player::CGuardState(CPlayerState::STATETYPE::STATETYPE_START);
					pPlayer->Set_State(pPlayer->Get_State()->ChangeState(pPlayer, pPlayer->Get_State(), pGuardState));
					pPlayer->Spawn_GuardEffect();
				}
				else
					pDamaged->Take_Damage(pMoriblinSpear->Get_Stats().m_fAttackPower, nullptr, m_tProjectileDesc.pOwner);
			}
		}

		m_bShouldDestroy = true;
		pMoriblinSpear->Set_IsProjectileAlive(false);
	}
}

void CProjectile::BossblinSpear_Collision()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<CGameObject*> pDamagedObjects;
	pGameInstance->Collision_Check_Group_Multi(m_tProjectileDesc.bIsPlayerProjectile ? CCollision_Manager::COLLISION_MONSTER : CCollision_Manager::COLLISION_PLAYER,
		Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
	RELEASE_INSTANCE(CGameInstance);

	if (!pDamagedObjects.empty())
	{
		CBossblin* pBossblin = dynamic_cast<CBossblin*>(m_tProjectileDesc.pOwner);
		if (!pBossblin)
			return;

		for (auto& pDamaged : pDamagedObjects)
		{
			if (m_tProjectileDesc.bIsPlayerProjectile)
				pDamaged->Take_Damage(pBossblin->Get_Stats().m_fAttackPower, nullptr, m_tProjectileDesc.pOwner);
			else
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pDamaged);
				if (!pPlayer)
					continue;

				if (pPlayer->Get_State()->Get_StateId() == CPlayerState::STATE_ID::STATE_GUARD ||
					pPlayer->Get_State()->Get_StateId() == CPlayerState::STATE_ID::STATE_GUARD_MOVE)
				{
					pPlayer->Get_Model()->Reset_CurrentAnimation();
					CPlayerState* pGuardState = new Player::CGuardState(CPlayerState::STATETYPE::STATETYPE_START);
					pPlayer->Set_State(pPlayer->Get_State()->ChangeState(pPlayer, pPlayer->Get_State(), pGuardState));
					pPlayer->Spawn_GuardEffect();
				}
				else
					pDamaged->Take_Damage(pBossblin->Get_Stats().m_fAttackPower, nullptr, m_tProjectileDesc.pOwner);
			}
		}

		m_bShouldDestroy = true;
		pBossblin->Set_IsProjectileAlive(false);
	}
}

void CProjectile::PlayerBomb_Collision()
{
	if (!m_bShouldDestroy)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<CGameObject*> pDamagedObjects;
	pGameInstance->Collision_Check_Group_Multi(m_tProjectileDesc.bIsPlayerProjectile ? CCollision_Manager::COLLISION_MONSTER : CCollision_Manager::COLLISION_PLAYER,
		Get_Collider(CCollider::AIM::AIM_DAMAGE_OUTPUT), CCollider::AIM::AIM_DAMAGE_INPUT, pDamagedObjects);
	RELEASE_INSTANCE(CGameInstance);

	if (!pDamagedObjects.empty())
	{
		for (auto& pDamaged : pDamagedObjects)
		{
			if (m_tProjectileDesc.bIsPlayerProjectile)
			{
				pDamaged->Take_Damage(3.f, nullptr, m_tProjectileDesc.pOwner);

				Spawn_HitEffect(pDamaged);
			}
		}
	}
}

void CProjectile::Spawn_BombEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_BOMB_FLASH;
	tEffectDesc.m_fEffectLifespan = .2f; 
	tEffectDesc.m_pOwner = this;
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	/* Spawn Bomb Flash Effect (Rect Buffer). */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Bomb_Flash_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_BOMB_EXPLOSION;
	tEffectDesc.m_fEffectLifespan = .25f;

	_uint iExplosionCounter = 0;
	while (iExplosionCounter <= 5)
	{
		_float fRandX = ((_float)rand() / (float)(RAND_MAX)) * .25;
		_float fRandY = ((_float)rand() / (float)(RAND_MAX)) * .25;
		_bool bSignX = rand() % 2;
		_bool bSignY = rand() % 2;

		_matrix mWorldMatrix = m_pTransformCom->Get_WorldMatrix();
		_matrix mMatrixTranslation = XMMatrixTranslation(bSignX ? fRandX : -fRandX, bSignY ? fRandY : -fRandY, 0.f);
		mWorldMatrix *= mMatrixTranslation;

		XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);

		/* Spawn Bomb Exposion Effect (Rect Buffer). */
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Bomb_Explosion_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
			return;

		iExplosionCounter++;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CProjectile::Spawn_HitEffect(CGameObject*& pDamaged)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_HIT_FLASH;
	tEffectDesc.m_fEffectLifespan = .25f;
	tEffectDesc.m_pOwner = m_tProjectileDesc.pOwner;

	_matrix mWorldMatrix = XMMatrixIdentity();

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vDamagedPosition = ((CActor*)pDamaged)->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);

	_vector vDir = vPosition - vDamagedPosition;
	vDir = XMVector4Normalize(vDir);

	_vector vSpawnPosition = vDamagedPosition + (vDir * pDamaged->Get_Radius());
	memcpy(&mWorldMatrix.r[3], &vSpawnPosition, sizeof(_vector));

	_matrix mMatrixTranslation = XMMatrixTranslation(0.f, 0.5f, 0.f);
	mWorldMatrix *= mMatrixTranslation;

	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, mWorldMatrix);

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

void CProjectile::MoriblinSpear_Tick(_float fTimeDelta)
{
	m_pTransformCom->Move_Straight(fTimeDelta);

	if (m_fAliveTimer > 1.5f)
	{
		m_bShouldDestroy = true;

		CMoriblinSpear* pMoriblinSpear = dynamic_cast<CMoriblinSpear*>(m_tProjectileDesc.pOwner);
		if (pMoriblinSpear)
			pMoriblinSpear->Set_IsProjectileAlive(false);
	}
	else
		m_fAliveTimer += fTimeDelta;
}

void CProjectile::BossblinSpear_Tick(_float fTimeDelta)
{
	m_pTransformCom->Move_Straight(fTimeDelta);

	if (m_fAliveTimer > 1.5f)
	{
		m_bShouldDestroy = true;

		CBossblin* pBossblin = dynamic_cast<CBossblin*>(m_tProjectileDesc.pOwner);
		if (pBossblin)
			pBossblin->Set_IsProjectileAlive(false);
	}
	else
		m_fAliveTimer += fTimeDelta;
}

void CProjectile::PlayerBomb_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bShouldStartCountdown)
	{
		_float4 vPosition;

		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
		XMStoreFloat4(&vPosition, XMLoadFloat4(&vPosition) + XMVector4Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta * m_fBouncePower);
		XMStoreFloat4(&vPosition, XMLoadFloat4(&vPosition) + XMVector4Normalize(XMLoadFloat4(&m_vProjectileDirection)) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta);

		if (m_pNavigationCom->CanMove(XMLoadFloat4(&vPosition)))
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPosition));
		else
			XMStoreFloat4(&m_vProjectileDirection, (XMLoadFloat4(&m_vProjectileDirection) * -1));

		_float fNavigationHeight = m_pNavigationCom->Get_NavigationHeight(_float3(vPosition.x, vPosition.y, vPosition.z));
		m_fBouncePower -= 0.05f;

		if (vPosition.y <= fNavigationHeight + 0.1f)
		{
			if (m_iBounces < m_iMaxBounces)
			{
				m_iBounces++;
				m_fBouncePower = 2.f / (m_iBounces + 1);
			}
			else
			{
				m_bShouldStartCountdown = true;

				m_eShaderPass = VTXMODELPASS::VTXMODEL_COUNTDOWN;
			}
		}
		else
			m_fBouncePower -= 0.05f;
	}
	else
	{
		if (m_fCountdownTimer < m_fCountdownLifetime)
			m_fCountdownTimer += fTimeDelta;
		else
		{
			Spawn_BombEffect();

			m_bShouldDestroy = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

CProjectile* CProjectile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CProjectile* pInstance = new CProjectile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CStaticObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile::Clone(void * pArg)
{
	CProjectile* pInstance = new CProjectile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CProjectile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile::Free()
{
	__super::Free();
}