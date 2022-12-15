#include "stdafx.h"

#include "Item.h"
#include "GameInstance.h"
#include "Inventory.h"

CItem::CItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CItem::CItem(const CItem & rhs)
	: CActor(rhs)
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void * pArg)
{
	Setup_Item(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_FLOAT);
	
	_float4 vPosition; 
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	vPosition.y += .5f;
	m_pTransformCom->Set_State(CTransform::STATE::STATE_TRANSLATION, XMLoadFloat4(&vPosition));

	return S_OK;
}

_uint CItem::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	m_pModelCom->Play_Animation(fTimeDelta * 2, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	return S_OK;
}

_uint CItem::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (Check_Collision() == true)
	{
		CInventory::Get_Instance()->Add_Item(m_tItemDesc.m_eItemId);
		m_bShouldDestroy = true;
	}

	return S_OK;
}

HRESULT CItem::Render()
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

void CItem::Setup_Item(void * pArg)
{
	ZeroMemory(&m_tItemDesc, sizeof(ITEMDESC));
	memcpy(&m_tItemDesc, (ITEMDESC*)pArg, sizeof(ITEMDESC));
	
	switch ((ITEMID)m_tItemDesc.m_eItemId)
	{
	case ITEMID::ITEM_RUPEE_GREEN:
	case ITEMID::ITEM_BOMB:
		m_tItemDesc.m_eModelType = CModel::TYPE::TYPE_ANIM;
		m_tItemDesc.m_bAnimOnPickup = false;
		break;
	}
}

HRESULT CItem::Ready_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tItemDesc.mWorldMatrix;
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, m_tItemDesc.m_eModelType == CModel::TYPE::TYPE_ANIM 
		? TEXT("Prototype_Component_Shader_VtxAnimModel") 
		: TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, Get_ModelPrototypeId(m_tItemDesc.m_eItemId), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.m_bIsAttachedToBone = false;
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderItem"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::SetUp_ShaderResources()
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

	return S_OK;
}

_bool CItem::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
		case ANIMID::ANIM_FLOAT:
			return true;
		default:
			return true;
	}
}

_tchar * CItem::Get_ModelPrototypeId(ITEMID eItemId)
{
	switch ((ITEMID)eItemId)
	{
	case ITEMID::ITEM_RUPEE_GREEN:
		return TEXT("Prototype_Component_Model_RupeeGreen");
		break;
	case ITEMID::ITEM_BOMB:
		return TEXT("Prototype_Component_Model_Bomb");
		break;
	}
}

_bool CItem::Check_Collision()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	CGameObject* pGameObject = nullptr;
	_bool bCollided = pGameInstance->Collision_with_Group(CCollision_Manager::COLLISION_PLAYER, Get_Collider(CCollider::AIM::AIM_OBJECT), CCollider::AIM::AIM_DAMAGE_INPUT, pGameObject);

	RELEASE_INSTANCE(CGameInstance);

	return bCollided;
}

CItem* CItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void * pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();
}