#include "stdafx.h"

#include "MoriblinSword.h"
#include "GameInstance.h"
#include "MoriblinSwordState.h"
#include "MoriblinSwordIdleState.h"
#include "MoriblinSwordHitState.h"

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

	return S_OK;
}

HRESULT CMoriblinSword::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Rotation(_float3(0.f, 180.f, 0.f));
	m_fRadius = .5f;

	CMoriblinSwordState* pState = new CIdleState();
	m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);

	return S_OK;
}

_uint CMoriblinSword::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	AI_Behavior();
	TickState(fTimeDelta);
	 
	return OBJ_NOEVENT;
}

void CMoriblinSword::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pRendererCom && m_bIsInFrustum)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	LateTickState(fTimeDelta);
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	Render_Colliders();

	return E_NOTIMPL;
}

_float CMoriblinSword::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage > 0.f)
	{
		CMoriblinSwordState* pState = new CHitState(DamageCauser->Get_Position());
		m_pMoriblinSwordState = m_pMoriblinSwordState->ChangeState(this, m_pMoriblinSwordState, pState);
	}

	return 0.f;
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
	TransformDesc.fSpeedPerSec = 1.5f;
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
	ColliderDesc.vScale = _float3(1.4f, 1.4f, 1.2f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	CNavigation::NAVDESC NavDesc;
	ZeroMemory(&NavDesc, sizeof(CNavigation::NAVDESC));
	XMStoreFloat3(&NavDesc.vInitialPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Field"), (CComponent**)&m_pNavigationCom, &NavDesc)))
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