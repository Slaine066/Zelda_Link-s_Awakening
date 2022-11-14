#include "stdafx.h"

#include "MoriblinSword.h"
#include "GameInstance.h"

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

	m_pModelCom->Set_CurrentAnimIndex(ANIM_WAIT);

	return S_OK;
}

_uint CMoriblinSword::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// Execute Action based on STATE.
	Execute_State(fTimeDelta);

	m_bIsAnimationFinished = false;
	m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	// Change STATE when Animation ends.
	Reset_State();
	 
	return OBJ_NOEVENT;
}

void CMoriblinSword::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_bool bIsInFrustum = pGameInstance->IsIn_Frustum(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), .5f);

	if (m_pRendererCom && bIsInFrustum)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	
	RELEASE_INSTANCE(CGameInstance);
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
	m_eCurrentState = STATEID::STATE_DAMAGED;
	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_DAMAGE_FRONT);

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

void CMoriblinSword::Execute_State(_float fTimeDelta)
{
}

void CMoriblinSword::Reset_State()
{
	if (m_bIsAnimationFinished)
	{
		switch ((ANIMID)m_pModelCom->Get_CurrentAnimIndex())
		{
		case ANIMID::ANIM_DAMAGE_FRONT:
			m_eCurrentState = STATE_IDLE;
			m_pModelCom->Set_CurrentAnimIndex(ANIM_WAIT);
		}
	}
}

_bool CMoriblinSword::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_WAIT:
	case ANIM_WALK:
		return true;
	case ANIM_DAMAGE_BACK:
	case ANIM_DAMAGE_FRONT:
	case ANIM_DEAD_BACK:
	case ANIM_DEAD_FRONT:
	case ANIM_DEAD_FIRE: 
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
}