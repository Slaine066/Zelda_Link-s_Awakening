#include "stdafx.h"

#include "MoriblinSword.h"
#include "GameInstance.h"
#include "MoriblinSwordState.h"
#include "MoriblinSwordIdleState.h"
#include "MoriblinSwordHitState.h"
#include "MoriblinSwordDieState.h"
#include "MoriblinSwordGuardState.h"
#include "MoriblinSwordAttackState.h"
#include "MoriblinSwordFallState.h"
#include "Effect.h"
#include "Level_MoriblinCave.h"

using namespace MoriblinSword;

CMoriblinSword::CMoriblinSword(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMoriblinSword::CMoriblinSword(const CMoriblinSword & rhs)
	: CMonster(rhs)
{
}

HRESULT CMoriblinSword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_tStats.m_fMaxHp = 2;
	m_tStats.m_fCurrentHp = m_tStats.m_fMaxHp;
	m_tStats.m_fAttackPower = .5f;
	m_tStats.m_fWalkSpeed = .5f;
	m_tStats.m_fRunSpeed = 1.f;

	return S_OK;
}

HRESULT CMoriblinSword::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRadius = .25f;
	m_fAggroRadius = 2.f;
	m_fPatrolRadius = 2.f;
	m_fAttackRadius = .7f;

	CMoriblinSwordState* pState = new CIdleState();
	m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);

	return S_OK;
}

_uint CMoriblinSword::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
	{
		DropItems();
		return OBJ_DESTROY;
	}

	AI_Behavior();
	TickState(fTimeDelta);
	 
	return OBJ_NOEVENT;
}

_uint CMoriblinSword::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom && m_bIsInFrustum)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	LateTickState(fTimeDelta);

	HandleFall(fTimeDelta);

	return OBJ_NOEVENT;
}

HRESULT CMoriblinSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

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

	Render_Colliders();

	return E_NOTIMPL;
}

_float CMoriblinSword::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage > 0.f)
	{
		if ((m_pMoriblinSwordState->Get_StateId() == CMoriblinSwordState::STATE_ID::STATE_IDLE ||
			m_pMoriblinSwordState->Get_StateId() == CMoriblinSwordState::STATE_ID::STATE_ATTACK ||
			m_pMoriblinSwordState->Get_StateId() == CMoriblinSwordState::STATE_ID::STATE_GUARD) && m_pMoriblinSwordState->Has_Aggro())
		{
			m_pModelCom->Reset_CurrentAnimation();
			CMoriblinSwordState* pState = new CGuardState();
			m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);

			fDamage = 0.f;
		}
		else
		{
			if (m_tStats.m_fCurrentHp - fDamage <= 0.f)
			{
				m_tStats.m_fCurrentHp = 0.f;

				m_pModelCom->Reset_CurrentAnimation();
				CMoriblinSwordState* pState = new CDieState(DamageCauser->Get_Position());
				m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);

				/* If this Monster is in a Dungeon, remove it from Dungeon Room Monsters. */
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CLevel_MoriblinCave* pDungeonLevel = dynamic_cast<CLevel_MoriblinCave*>(pGameInstance->Get_CurrentLevel());
				if (pDungeonLevel)
					pDungeonLevel->Remove_MonsterFromRoom(this);
			}
			else
			{
				m_tStats.m_fCurrentHp -= fDamage;

				m_pModelCom->Reset_CurrentAnimation();
				CMoriblinSwordState* pState = new CHitState(DamageCauser->Get_Position());
				m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);
			}
		}
	}

	return fDamage;
}

void CMoriblinSword::Spawn_GuardEffect()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect::EFFECTDESC tEffectDesc;
	ZeroMemory(&tEffectDesc, sizeof(CEffect::EFFECTDESC));
	tEffectDesc.m_eEffectType = CEffect::EFFECT_TYPE::EFFECT_GUARD_RING;
	tEffectDesc.m_fEffectLifespan = .3f;
	tEffectDesc.m_pOwner = this;

	CHierarchyNode* m_pSocket = m_pModelCom->Get_BonePtr("attach_R");
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

HRESULT CMoriblinSword::Ready_Components(void * pArg)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_INPUT;
	ColliderDesc.vScale = _float3(1.f, 1.4f, 1.3f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_OUTPUT;
	ColliderDesc.vScale = _float3(.3f, .2f, 1.4f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, -.5f);
	ColliderDesc.m_bIsAttachedToBone = true;
	ColliderDesc.pSocket = m_pModelCom->Get_BonePtr("attach_L");
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

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoriblinSword::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	/* Hit */
	if (FAILED(m_pShaderCom->Set_RawValue("g_HitTimer", &m_fHitTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_HitLifespan", &m_fHitLifespan, sizeof(_float))))
		return E_FAIL;

	/* Dissolve */
	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DissolveTexture", m_pDissolveTextureCom->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveTimer", &m_fDissolveTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolveLifespan", &m_fDissolveLifespan, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMoriblinSword::AI_Behavior()
{
	CMoriblinSwordState* pNewState = m_pMoriblinSwordState->AI_Behavior(this);
	if (pNewState)
		m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pNewState);
}

void CMoriblinSword::TickState(_float fTimeDelta)
{
	CMoriblinSwordState* pNewState = m_pMoriblinSwordState->Tick(this, fTimeDelta);
	if (pNewState)
		m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pNewState);
}

void CMoriblinSword::LateTickState(_float fTimeDelta)
{
	CMoriblinSwordState* pNewState = m_pMoriblinSwordState->LateTick(this, fTimeDelta);
	if (pNewState)
		m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pNewState);
}

void CMoriblinSword::HandleFall(_float fTimeDelta)
{
	/* Check if MoriblinSword is currently on a NOBLOCK Cell. */
	if (m_pNavigationCom->Get_CellType() == CCell::CELL_TYPE::CELL_NOBLOCK && m_pMoriblinSwordState->Get_StateId() != CMoriblinSwordState::STATE_ID::STATE_FALL)
	{
		m_pModelCom->Reset_CurrentAnimation();
		CMoriblinSwordState* pState = new CFallState();
		m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);
	}
}

_bool CMoriblinSword::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_DEAD_FIRE:
	case ANIM_FALL:
	case ANIM_PIYO:
	case ANIM_STANCE_WAIT:
	case ANIM_STANCE_WALK:
	case ANIM_WAIT:
	case ANIM_WALK:
		return true;
	case ANIM_DAMAGE_BACK:
	case ANIM_DAMAGE_FRONT:
	case ANIM_DEAD_BACK:
	case ANIM_DEAD_FRONT:
	case ANIM_FIND:
	case ANIM_GUARD:
	case ANIM_KYOROKYORO:
	case ANIM_STAGGER:
		return false;
	}
}

CMoriblinSword * CMoriblinSword::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMoriblinSword* pInstance = new CMoriblinSword(pDevice, pContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CMoriblinSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoriblinSword::Clone(void * pArg)
{
	CMoriblinSword* pInstance = new CMoriblinSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CMoriblinSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoriblinSword::Free()
{
	__super::Free();

	Safe_Delete(m_pMoriblinSwordState);
}