#include "stdafx.h"

#include "MoriblinSpear.h"
#include "GameInstance.h"
#include "MoriblinSpearState.h"
#include "MoriblinSpearIdleState.h"
#include "MoriblinSpearHitState.h"
#include "MoriblinSpearDieState.h"
#include "Weapon.h"
#include "MoriblinSpearFallState.h"

using namespace MoriblinSpear;

CMoriblinSpear::CMoriblinSpear(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CMoriblinSpear::CMoriblinSpear(const CMoriblinSpear & rhs)
	: CMonster(rhs)
{
}

HRESULT CMoriblinSpear::Initialize_Prototype()
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

HRESULT CMoriblinSpear::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_fRadius = .25f;
	m_fAggroRadius = 2.f;
	m_fPatrolRadius = 2.f;
	m_fAttackRadius = 2.f;

	CMoriblinSpearState* pState = new CIdleState();
	m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pState);

	return S_OK;
}

_uint CMoriblinSpear::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
	{
		DropItems();
		return OBJ_DESTROY;
	}

	for (auto& pParts : m_vParts)
		pParts->Tick(fTimeDelta);

	AI_Behavior();
	TickState(fTimeDelta);

	return OBJ_NOEVENT;
}

_uint CMoriblinSpear::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom && m_bIsInFrustum)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		if (!m_bIsProjectileAlive)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_vParts[PARTS_SPEAR]);
	}

	for (auto& pParts : m_vParts)
		pParts->Late_Tick(fTimeDelta);

	LateTickState(fTimeDelta);

	HandleFall(fTimeDelta);

	return OBJ_NOEVENT;
}

HRESULT CMoriblinSpear::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	Render_Colliders();

	return S_OK;
}

_float CMoriblinSpear::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage > 0.f)
	{
		if (m_tStats.m_fCurrentHp - fDamage <= 0.f)
		{
			m_tStats.m_fCurrentHp = 0.f;

			m_pModelCom->Reset_CurrentAnimation();
			CMoriblinSpearState* pState = new CDieState(DamageCauser->Get_Position());
			m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pState);
		}
		else
		{
			m_tStats.m_fCurrentHp -= fDamage;

			m_pModelCom->Reset_CurrentAnimation();
			CMoriblinSpearState* pState = new CHitState(DamageCauser->Get_Position());
			m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pState);
		}
	}

	return 0.f;
}

HRESULT CMoriblinSpear::Ready_Parts()
{
	m_vParts.resize(PARTS_END);

	/* For.Spear */
	CHierarchyNode*	pSocket = m_pModelCom->Get_BonePtr("attach_L");
	if (!pSocket)
		return E_FAIL;

	CWeapon::WEAPONDESC WeaponDesc;
	WeaponDesc.pSocket = pSocket;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_World4x4Ptr();
	WeaponDesc.SocketPivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.bIsPlayerWeapon = false;
	WeaponDesc.pModelPrototypeId = TEXT("Prototype_Component_Model_Spear");

	CCollider::COLLIDERDESC tColliderDesc;
	ZeroMemory(&tColliderDesc, sizeof(CCollider::COLLIDERDESC));
	tColliderDesc.eAim = CCollider::AIM::AIM_DAMAGE_OUTPUT;
	tColliderDesc.vScale = _float3(.3f, .2f, 1.5f);
	tColliderDesc.vPosition = _float3(0.f, 0.f, .25f);

	WeaponDesc.tColliderDesc = tColliderDesc;
	
	Safe_AddRef(pSocket);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_vParts[PARTS_SPEAR] = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (!m_vParts[PARTS_SPEAR])
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMoriblinSpear::Ready_Components(void * pArg)
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
	ColliderDesc.vScale = _float3(1.4f, 1.4f, 1.2f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
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

HRESULT CMoriblinSpear::SetUp_ShaderResources()
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

void CMoriblinSpear::AI_Behavior()
{
	CMoriblinSpearState* pNewState = m_pMoriblinSpearState->AI_Behavior(this);
	if (pNewState)
		m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pNewState);
}

void CMoriblinSpear::TickState(_float fTimeDelta)
{
	CMoriblinSpearState* pNewState = m_pMoriblinSpearState->Tick(this, fTimeDelta);
	if (pNewState)
		m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pNewState);
}

void CMoriblinSpear::LateTickState(_float fTimeDelta)
{
	CMoriblinSpearState* pNewState = m_pMoriblinSpearState->LateTick(this, fTimeDelta);
	if (pNewState)
		m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pNewState);
}

void CMoriblinSpear::HandleFall(_float fTimeDelta)
{
	/* Check if MoriblinSpear is currently on a NOBLOCK Cell. */
	if (m_pNavigationCom->Get_CellType() == CCell::CELL_TYPE::CELL_NOBLOCK && m_pMoriblinSpearState->Get_StateId() != CMoriblinSpearState::STATE_ID::STATE_FALL)
	{
		m_pModelCom->Reset_CurrentAnimation();
		CMoriblinSpearState* pState = new CFallState();
		m_pMoriblinSpearState = m_pMoriblinSpearState->ChangeState(this, m_pMoriblinSpearState, pState);
	}
}

_bool CMoriblinSpear::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_DEAD_FIRE:
	case ANIM_FALL:
	case ANIM_PIYO:
	case ANIM_STANCE_WAIT:
	case ANIM_STANCE_WALK_BACK:
	case ANIM_STANCE_WALK_FRONT:
	case ANIM_STANCE_WALK_LEFT:
	case ANIM_STANCE_WALK_RIGHT:
	case ANIM_WAIT:
	case ANIM_WALK:
		return true;
	case ANIM_DAMAGE_BACK:
	case ANIM_DAMAGE_FRONT:
	case ANIM_DEAD_BACK:
	case ANIM_DEAD_FRONT:
	case ANIM_FIND:
	case ANIM_KYOROKYORO:
	case ANIM_SPEAR_THROW:
		return false;
	}
}

CMoriblinSpear * CMoriblinSpear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMoriblinSpear* pInstance = new CMoriblinSpear(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CMoriblinSpear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoriblinSpear::Clone(void * pArg)
{
	CMoriblinSpear* pInstance = new CMoriblinSpear(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CMoriblinSpear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoriblinSpear::Free()
{
	__super::Free();
	
	Safe_Delete(m_pMoriblinSpearState);

	for (auto& pGameObject : m_vParts)
		Safe_Release(pGameObject);

	m_vParts.clear();
}