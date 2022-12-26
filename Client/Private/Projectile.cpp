#include "stdafx.h"

#include "Projectile.h"
#include "GameInstance.h"
#include "MoriblinSpear.h"
#include "Bossblin.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerGuardState.h"

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

	if (m_tProjectileDesc.eProjectileType == PROJECTILE_TYPE::PROJECTILE_MORIBLINSPEAR ||
		m_tProjectileDesc.eProjectileType == PROJECTILE_TYPE::PROJECTILE_BOSSBLINSPEAR)
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

		_vector vTargetPosition = XMVectorSet(m_tProjectileDesc.vTargetPosition.x, vPosition.y, m_tProjectileDesc.vTargetPosition.z, 1.f);

		_vector vLook = vTargetPosition - XMLoadFloat4(&vPosition);
		m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, XMVector3Normalize(vLook) * m_pTransformCom->Get_Scale(CTransform::STATE::STATE_LOOK));
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
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

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectile::SetUp_ShaderResources()
{
	if (!m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

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