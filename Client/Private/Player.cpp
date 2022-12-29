#include "stdafx.h"

#include "Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "PlayerState.h"
#include "PlayerIdleState.h"
#include "PlayerHitState.h"
#include "PlayerFallState.h"
#include "PlayerAchieveState.h"
#include "PlayerCarryState.h"
#include "PlayerJumpState.h"
#include "Layer.h"
#include "UI_Manager.h"
#include "Effect.h"

using namespace Player;

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_fRadius = 0.2f;

	m_tStats.m_fMaxHp = 4;
	m_tStats.m_fCurrentHp = m_tStats.m_fMaxHp;
	m_tStats.m_fAttackPower = 1; /* Set to 0 once Equipment gets implemented. Increase based on the Equipped Weapon. */
	m_tStats.m_fWalkSpeed = .7f;
	m_tStats.m_fRunSpeed = 1.4f;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tStats.m_fMaxHp = CUI_Manager::Get_Instance()->Get_MaxHp() == 0 ? 4 : CUI_Manager::Get_Instance()->Get_MaxHp();
	m_tStats.m_fCurrentHp = CUI_Manager::Get_Instance()->Get_CurrentHp() == 0 ? m_tStats.m_fMaxHp : CUI_Manager::Get_Instance()->Get_CurrentHp();

	CPlayerState* pState = new CIdleState();
	m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);

	return S_OK;
}

_uint CPlayer::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_PLAYER, this);

	HandleInvincibility(fTimeDelta);

	HandleInput();
	TickState(fTimeDelta);		

	return OBJ_NOEVENT;
}

_uint CPlayer::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	LateTickState(fTimeDelta);
	HandleFall(fTimeDelta);

	/* Reset HIT Shader Pass. */
	if (m_eShaderPass == VTXANIMMODELPASS::VTXANIMMODEL_HIT)
	{
		if (m_fHitTimer > m_fHitLifespan)
		{
			m_eShaderPass = VTXANIMMODELPASS::VTXANIMMODEL_DEFAULT;
			m_fHitTimer = 0.f;
		}
		else
			m_fHitTimer += fTimeDelta;
	}

	return OBJ_NOEVENT;
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == MESH_SWORD_B || i == MESH_SWORD_B_HANDLE|| i == MESH_SHIELD_B|| i == MESH_SHIELD_B_MIRROR|| i == MESH_MAGIC_ROD ||
			i == MESH_HOOKSHOT || i == MESH_OCARINA || i == MESH_SHOVEL || i == MESH_FLIPPERS)
			continue;

		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->SetUp_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderPass)))
			return E_FAIL;
	}

	// Only in Debug
	Render_Colliders();
	Render_NavigationMesh();

	return S_OK;
}

_float CPlayer::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage > 0.f && !m_bIsInvincible && 
		m_pPlayerState->Get_StateId() != CPlayerState::STATE_ID::STATE_FALL && 
		m_pPlayerState->Get_StateId() != CPlayerState::STATE_ID::STATE_DIE)
	{ 
		if (m_tStats.m_fCurrentHp - fDamage <= 0.f)
		{
			m_tStats.m_fCurrentHp = 0.f;

			/* TODO: Implement Death. */
			m_pModelCom->Reset_CurrentAnimation();
			CPlayerState* pState = new CHitState(DamageCauser->Get_Position());
			m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);

			m_bIsInvincible = true;
		}
		else
		{
			m_tStats.m_fCurrentHp -= fDamage;

			m_pModelCom->Reset_CurrentAnimation();
			CPlayerState* pState = new CHitState(DamageCauser->Get_Position());
			m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);

			m_bIsInvincible = true;
		}
	}

	return 0.f;
}

HRESULT CPlayer::Ready_Components(void* pArg)
{
	memcpy(&m_tModelDesc, pArg, sizeof(MODELDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tModelDesc.mWorldMatrix;
	TransformDesc.fSpeedPerSec = m_tStats.m_fRunSpeed;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_INPUT;
	ColliderDesc.vScale = _float3(.8f, 1.4f, .8f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderPlayer"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_OUTPUT;
	ColliderDesc.vScale = _float3(.2f, .2f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, -.35f);
	ColliderDesc.m_bIsAttachedToBone = true;
	ColliderDesc.pSocket = m_pModelCom->Get_BonePtr("itemA_L");
	ColliderDesc.pPivotMatrix = m_pModelCom->Get_PivotFloat4x4();

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderSword"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVDESC NavDesc;
	ZeroMemory(&NavDesc, sizeof(CNavigation::NAVDESC));
	XMStoreFloat3(&NavDesc.vInitialPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
	
	_uint iLevelIndex = CGameInstance::Get_Instance()->Get_NextLevelIndex();

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), iLevelIndex, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NavDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_HitTimer", &m_fHitTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_HitLifespan", &m_fHitLifespan, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CPlayerState * CPlayer::Get_State()
{
	return m_pPlayerState;
}

void CPlayer::Set_AchieveState()
{
	m_pModelCom->Reset_CurrentAnimation();
	CPlayerState* pState = new CAchieveState(CPlayerState::STATETYPE::STATETYPE_START);
	m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);
}

_bool CPlayer::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_CARRY:
	case ANIM_IDLE:
	case ANIM_IDLE_BARE:
	case ANIM_IDLE_CARRY:
	case ANIM_ITEM_GET_LOOP:
	case ANIM_LADDER_DOWN:
	case ANIM_LADDER_UP:
	case ANIM_LADDER_WAIT:
	case ANIM_MOVE_CARRY:
	case ANIM_PULL:
	case ANIM_PULL_IDLE:
	case ANIM_PUSH:
	case ANIM_PUSH_IDLE:
	case ANIM_RUN:
	case ANIM_RUN_BARE:
	case ANIM_SHIELD_HOLD_BACK:
	case ANIM_SHIELD_HOLD_FRONT:
	case ANIM_SHIELD_HOLD_LEFT:
	case ANIM_SHIELD_HOLD_RIGHT:
	case ANIM_SHIELD_LOOP:
	case ANIM_SLASH_HOLD_BACK:
	case ANIM_SLASH_HOLD_FRONT:
	case ANIM_SLASH_HOLD_LEFT:
	case ANIM_SLASH_HOLD_LOOP:
	case ANIM_SLASH_HOLD_RIGHT:
	case ANIM_TALK:
	case ANIM_WALK:
	case ANIM_WALK_BARE:
		return true;
	case ANIM_BOW_END:
	case ANIM_BOW_START:
	case ANIM_CARRY_FAIL:
	case ANIM_DEATH:
	case ANIM_DAMAGE_AIR:
	case ANIM_DAMAGE_BACK:
	case ANIM_DAMAGE_FRONT:
	case ANIM_FALL_FROM_TOP:
	case ANIM_FALL_HOLE:
	case ANIM_ITEM_CARRY:
	case ANIM_ITEM_GET_END:
	case ANIM_ITEM_GET_START:
	case ANIM_ITEM_PUT:
	case ANIM_JUMP:
	case ANIM_JUMP_SLASH:
	case ANIM_KEY_OPEN:
	case ANIM_LADDER_DOWN_START:
	case ANIM_LADDER_UP_END:
	case ANIM_LADDER_UP_START:
	case ANIM_LAND:
	case ANIM_PULL_START:
	case ANIM_PUSH_START:
	case ANIM_REVODOOR_IN:
	case ANIM_ROD_END:
	case ANIM_ROD_START:
	case ANIM_SHIELD_END:
	case ANIM_SHIELD_START:
	case ANIM_SLASH:
	case ANIM_SLASH_HOLD_END:
	case ANIM_SLASH_HOLD_START:
	case ANIM_THROW:
	case ANIM_WARP_END:
	case ANIM_WARP_START:
		return false;
	}
}

void CPlayer::HandleInput()
{
	CPlayerState* pNewState = m_pPlayerState->HandleInput(this);
	if (pNewState)
		m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pNewState);
}

void CPlayer::TickState(_float fTimeDelta)
{
	CPlayerState* pNewState = m_pPlayerState->Tick(this, fTimeDelta);
	if (pNewState)
		m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pNewState);
}

void CPlayer::LateTickState(_float fTimeDelta)
{
	CPlayerState* pNewState = m_pPlayerState->LateTick(this, fTimeDelta);
	if (pNewState)
		m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pNewState);
}

void CPlayer::HandleInvincibility(_float fTimeDelta)
{
	if (m_bIsInvincible)
	{
		m_fInvincibleTimer += fTimeDelta;

		if (m_fInvincibleTimer > 1.f) /* 1 second of Invincibility every time you get hit. */
		{
			m_bIsInvincible = false;
			m_fInvincibleTimer = 0.f;
		}
	}
}

void CPlayer::HandleFall(_float fTimeDelta)
{
	/* Check if Player is currently on a NOBLOCK Cell. */
	if (m_pNavigationCom->Get_CellType() == CCell::CELL_TYPE::CELL_NOBLOCK && m_pPlayerState->Get_StateId() != CPlayerState::STATE_ID::STATE_FALL)
	{
		m_pModelCom->Reset_CurrentAnimation();
		CPlayerState* pState = new CFallState();
		m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);
	}
}

CPlayerState* CPlayer::Use_Item(_bool bIsX)
{
	INVENTORYOBJDESC* tInvObjDesc = nullptr;

	if (bIsX)
		tInvObjDesc = CInventory::Get_Instance()->Get_ItemX();
	else
		tInvObjDesc = CInventory::Get_Instance()->Get_ItemY();

	if (!tInvObjDesc)
		return nullptr;

	CPlayerState* pNewState = nullptr;

	/* Change State based on Item used. */
	switch (tInvObjDesc->m_eItemId)
	{
	case ITEMID::ITEM_BOMB:
		pNewState = new CCarryState(CPlayerState::STATETYPE::STATETYPE_START);
		break;
	case ITEMID::ITEM_ROCFEATHER:
	{
		CPlayerState::STATE_ID eStateId = m_pPlayerState->Get_StateId();
		switch (eStateId)
		{
			case CPlayerState::STATE_ID::STATE_IDLE:
				pNewState = new CJumpState(CPlayerState::STATETYPE::STATETYPE_MAIN);
				break;
			case CPlayerState::STATE_ID::STATE_MOVE:
				pNewState =  new CJumpState(CPlayerState::STATETYPE::STATETYPE_MAIN, true);
				break;
		}
	}
	case ITEMID::ITEM_BOW:
		break;
	}

	/* Decrease Item Counter. */
	CInventory* pInventory = CInventory::Get_Instance();
	pInventory->Decrease_ItemCount(tInvObjDesc->m_eItemId, bIsX ? pInventory->Get_IndexItemX() : pInventory->Get_IndexItemY());

	return pNewState;
}

void CPlayer::Spawn_GuardEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_GUARD_RING;
	tEffectDesc.m_fEffectLifespan = .3f;
	tEffectDesc.m_pOwner = this;

	CHierarchyNode* m_pSocket = m_pModelCom->Get_BonePtr("hand_R");
	_matrix SocketMatrix = m_pSocket->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_pModelCom->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_pTransformCom->Get_World4x4());
	XMStoreFloat4x4(&tEffectDesc.m_WorldMatrix, SocketMatrix);

	/* Spawn Hit Ring Effect (Model) on Shield Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Guard_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_GUARD;
	tEffectDesc.m_fEffectLifespan = .15f;

	/* Spawn Guard Flash Effect (Model) on Shield Bone. */
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Guard_Effect"), TEXT("Prototype_GameObject_Effect"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), &tEffectDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

CPlayer* CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Delete(m_pPlayerState);
}