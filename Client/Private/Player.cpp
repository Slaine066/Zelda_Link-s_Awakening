#include "stdafx.h"

#include "Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "PlayerState.h"
#include "PlayerIdleState.h"
#include "PlayerHitState.h"

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

	m_tStats.m_fMaxHp = 4;
	m_tStats.m_fCurrentHp = m_tStats.m_fMaxHp;
	m_tStats.m_fAttackPower = 1;
	m_tStats.m_fWalkSpeed = .75f;
	m_tStats.m_fRunSpeed = 1.5f;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CPlayerState* pState = new CIdleState();
	m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);

	return S_OK;
}

_uint CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// Collision Handling
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_PLAYER, this);

	HandleInput();
	TickState(fTimeDelta);				

	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	LateTickState(fTimeDelta);
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	// Only in Debug
	Render_Colliders();
	Render_NavigationMesh();

	return S_OK;
}

_float CPlayer::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage > 0.f)
	{
		m_pModelCom->Reset_CurrentAnimation();
		CPlayerState* pState = new CHitState(DamageCauser->Get_Position());
		m_pPlayerState = m_pPlayerState->ChangeState(this, m_pPlayerState, pState);
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_INPUT;
	ColliderDesc.vScale = _float3(.8f, 1.3f, .8f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
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
	
	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Field"), (CComponent**)&m_pNavigationCom, &NavDesc)))
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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