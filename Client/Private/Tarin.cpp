#include "stdafx.h"

#include "Tarin.h"
#include "GameInstance.h"
#include "TarinState.h"
#include "TarinIdleState.h"

using namespace Tarin;

CTarin::CTarin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNpc(pDevice, pContext)
{
}

CTarin::CTarin(const CTarin & rhs)
	: CNpc(rhs)
{
}

HRESULT CTarin::Initialize_Prototype()
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

HRESULT CTarin::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRadius = .25f;

	CTarinState* pState = new CIdleState();
	m_pTarinState = m_pTarinState->ChangeState(this, m_pTarinState, pState);

	return S_OK;
}

_uint CTarin::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;


	AI_Behavior();
	TickState(fTimeDelta);

	return OBJ_NOEVENT;
}

_uint CTarin::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom && m_bIsInFrustum)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	LateTickState(fTimeDelta);

	return OBJ_NOEVENT;
}

HRESULT CTarin::Render()
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

	return S_OK;
}

HRESULT CTarin::Ready_Components(void * pArg)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_MARINHOUSE, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CNavigation::NAVDESC NavDesc;
	ZeroMemory(&NavDesc, sizeof(CNavigation::NAVDESC));
	XMStoreFloat3(&NavDesc.vInitialPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	_uint iLevelIndex = CGameInstance::Get_Instance()->Get_NextLevelIndex();

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation"), iLevelIndex, TEXT("Prototype_Component_Navigation"), (CComponent**)&m_pNavigationCom, &NavDesc)))
		return E_FAIL;

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(.8f, 1.4f, .8f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderBedBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTarin::SetUp_ShaderResources()
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

void CTarin::AI_Behavior()
{
	CTarinState* pNewState = m_pTarinState->AI_Behavior(this);
	if (pNewState)
		m_pTarinState = m_pTarinState->ChangeState(this, m_pTarinState, pNewState);
}

void CTarin::TickState(_float fTimeDelta)
{
	CTarinState* pNewState = m_pTarinState->Tick(this, fTimeDelta);
	if (pNewState)
		m_pTarinState = m_pTarinState->ChangeState(this, m_pTarinState, pNewState);
}

void CTarin::LateTickState(_float fTimeDelta)
{
	CTarinState* pNewState = m_pTarinState->LateTick(this, fTimeDelta);
	if (pNewState)
		m_pTarinState = m_pTarinState->ChangeState(this, m_pTarinState, pNewState);
}

_bool CTarin::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
		case ANIM_TALK:
		case ANIM_WAIT:
		case ANIM_WALK:
			return true;
	}
}

CTarin * CTarin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTarin* pInstance = new CTarin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CTarin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTarin::Clone(void * pArg)
{
	CTarin* pInstance = new CTarin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CTarin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarin::Free()
{
	__super::Free();

	Safe_Delete(m_pTarinState);
}