#include "stdafx.h"

#include "Bossblin.h"
#include "GameInstance.h"
#include "BossblinState.h"
#include "BossblinIdleState.h"
#include "BossblinHitState.h"
#include "BossblinDieState.h"
#include "BossblinGuardState.h"
#include "BossblinDownState.h"
#include "Weapon.h"
#include "PlayerState.h"
#include "PlayerGuardState.h"

using namespace Bossblin;

CBossblin::CBossblin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBossblin::CBossblin(const CBossblin & rhs)
	: CMonster(rhs)
{
}

HRESULT CBossblin::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_tStats.m_fMaxHp = 10;
	m_tStats.m_fCurrentHp = m_tStats.m_fMaxHp;
	m_tStats.m_fAttackPower = 1.f;
	m_tStats.m_fWalkSpeed = .5f;
	m_tStats.m_fRunSpeed = 1.4f;

	return S_OK;
}

HRESULT CBossblin::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_fRadius = .5f;
	m_fAggroRadius = 2.f;
	m_fAttackRadius = 2.f;

	CBossblinState* pState = new CIdleState();
	m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pState);

	return S_OK;
}

_uint CBossblin::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	for (auto& pParts : m_vParts)
		pParts->Tick(fTimeDelta);

	AI_Behavior();
	TickState(fTimeDelta);

	return OBJ_NOEVENT;
}

_uint CBossblin::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bIsInFrustum)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		if (m_bShouldRenderWeapon)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_vParts[PARTS_SPEAR]);
	}

	for (auto& pParts : m_vParts)
		pParts->Late_Tick(fTimeDelta);

	LateTickState(fTimeDelta);

	Check_Collision();

	return OBJ_NOEVENT;
}

HRESULT CBossblin::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	Render_Colliders();

	return S_OK;
}

_float CBossblin::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage > 0.f)
	{
		/* Take Damage only if Bossblin is in STATE_DOWN */
		if (m_pBossblinState->Get_StateId() == CBossblinState::STATE_ID::STATE_DOWN && m_pBossblinState->Get_StateType() == CBossblinState::STATETYPE::STATETYPE_MAIN ||
			m_pBossblinState->Get_StateId() == CBossblinState::STATE_ID::STATE_HIT)
		{
			if (m_tStats.m_fCurrentHp - fDamage <= 0.f)
			{
				m_tStats.m_fCurrentHp = 0.f;

				m_pModelCom->Reset_CurrentAnimation();
				CBossblinState* pState = new CDieState(CBossblinState::STATETYPE::STATETYPE_START);
				m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pState);
			}
			else
			{
				m_tStats.m_fCurrentHp -= fDamage;

				m_pModelCom->Reset_CurrentAnimation();

				switch (m_pBossblinState->Get_StateId())
				{
					case CBossblinState::STATE_ID::STATE_DOWN:
					{
						CDownState* pDownState = dynamic_cast<CDownState*>(m_pBossblinState);
						if (!pDownState)
							goto Damage;

						CBossblinState* pState = new CHitState(pDownState->Get_DownTimer());
						m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pState);
					}
						break;
					case CBossblinState::STATE_ID::STATE_HIT:
					{
						CHitState* pHitState = dynamic_cast<CHitState*>(m_pBossblinState);
						if (!pHitState)
							goto Damage;

						CBossblinState* pState = new CHitState(pHitState->Get_DownTimer());
						m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pState);
					}
						break;
				}
			}
		}
		else if (m_pBossblinState->Get_StateId() == CBossblinState::STATE_ID::STATE_ATTACK_TACKLE)
			goto Damage;
		else 
		{
			if (m_pBossblinState->Has_Aggro())
			{
				m_pModelCom->Reset_CurrentAnimation();
				CBossblinState* pState = new Bossblin::CGuardState();
				m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pState);
			}
		}
	}

Damage:
	return 0.f;
}

HRESULT CBossblin::Ready_Parts()
{
	m_vParts.resize(PARTS_END);

	/* For.Spear */
	CHierarchyNode*	pSocket = m_pModelCom->Get_BonePtr("attach_R");
	if (!pSocket)
		return E_FAIL;

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pSocket = pSocket;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	WeaponDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.bIsPlayerWeapon = false;
	WeaponDesc.pModelPrototypeId = TEXT("Prototype_Component_Model_BossblinSpear");

	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
	tColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_OUTPUT;
	tColliderDesc.vScale = _float3(.3f, .3f, 2.3f);
	tColliderDesc.vPosition = _float3(0.f, 0.f, .3f);

	WeaponDesc.tColliderDesc = tColliderDesc;

	Safe_AddRef(pSocket);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_vParts[PARTS_SPEAR] = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (!m_vParts[PARTS_SPEAR])
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBossblin::Ready_Components(void * pArg)
{
	memcpy(&m_tModelDesc, pArg, sizeof(MODELDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tModelDesc.mWorldMatrix;
	TransformDesc.fSpeedPerSec = m_tStats.m_fWalkSpeed;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_MORIBLINCAVE, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_INPUT;
	ColliderDesc.vScale = _float3(1.5f, 2.5f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 1.25f, 0.f);

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVDESC NavDesc;
	ZeroMemory(&NavDesc, sizeof(CNavigation::NAVDESC));
	XMStoreFloat3(&NavDesc.vInitialPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_MORIBLINCAVE, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NavDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossblin::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBossblin::AI_Behavior()
{
	CBossblinState* pNewState = m_pBossblinState->AI_Behavior(this);
	if (pNewState)
		m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pNewState);
}

void CBossblin::TickState(_float fTimeDelta)
{
	CBossblinState* pNewState = m_pBossblinState->Tick(this, fTimeDelta);
	if (pNewState)
		m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pNewState);
}

void CBossblin::LateTickState(_float fTimeDelta)
{
	CBossblinState* pNewState = m_pBossblinState->LateTick(this, fTimeDelta);
	if (pNewState)
		m_pBossblinState = m_pBossblinState->ChangeState(this, m_pBossblinState, pNewState);
}

void CBossblin::Check_Collision()
{
	if (m_pBossblinState->Get_StateId() == CBossblinState::STATE_ID::STATE_DOWN ||
		m_pBossblinState->Get_StateId() == CBossblinState::STATE_ID::STATE_HIT)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	vector<CGameObject*> vDamagedObjects;
	pGameInstance->Collision_Check_Group_Multi(CCollision_Manager::COLLISION_PLAYER, Get_Collider(CCollider::AIM_DAMAGE_INPUT), CCollider::AIM_DAMAGE_INPUT, vDamagedObjects);

	RELEASE_INSTANCE(CGameInstance);

	if (!vDamagedObjects.empty())
	{
		for (auto& pDamaged : vDamagedObjects)
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
			}
			else
				pDamaged->Take_Damage(m_tStats.m_fAttackPower, nullptr, this);
		}
	}
}

_bool CBossblin::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_DEAD:
	case ANIM_DOWN:
	case ANIM_MOVE_JUMP:
	case ANIM_TACKLE:
	case ANIM_WAIT:
		return true;
	case ANIM_BRAKE:
	case ANIM_DAMAGE:
	case ANIM_DEAD_START:
	case ANIM_DOWN_END:
	case ANIM_DOWN_START:
	case ANIM_GUARD:
	case ANIM_KYOROKYORO:
	case ANIM_SPEAR_THROW:
	case ANIM_TACKLE_END:
	case ANIM_TACKLE_START:
		return false;
	}
}

CBossblin * CBossblin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossblin* pInstance = new CBossblin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CBossblin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossblin::Clone(void * pArg)
{
	CBossblin* pInstance = new CBossblin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CBossblin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossblin::Free()
{
	__super::Free();

	Safe_Delete(m_pBossblinState);

	for (auto& pGameObject : m_vParts)
		Safe_Release(pGameObject);

	m_vParts.clear();
}