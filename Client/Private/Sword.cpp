#include "stdafx.h"

#include "Sword.h"
#include "GameInstance.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerAchieveState.h"
#include "Item.h"
#include "UI.h"

CSword::CSword(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractableObject(pDevice, pContext)
{
}

CSword::CSword(const CSword & rhs)
	: CInteractableObject(rhs)
{
}

HRESULT CSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSword::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteractableType = INTERACTABLE_SWORD;

	return S_OK;
}

_uint CSword::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	return S_OK;
}

_uint CSword::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (CanInteract())
		Spawn_InteractButton();
	else
	{
		if (m_pInteractButton)
		{
			m_pInteractButton->Set_ShouldDestroy(true);
			m_pInteractButton = nullptr;
		}
	}

	return S_OK;
}

HRESULT CSword::Render()
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

HRESULT CSword::Ready_Components(void* pArg)
{
	memcpy(&m_tModelDesc, pArg, sizeof(MODELDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tModelDesc.mWorldMatrix;
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Sword"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, .5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderSword"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(.5f, .5f, .5f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderSwordBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CSword::CanInteract()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	if (!pPlayer)
		return false;

	if (m_bDidInteract)
	{
		pPlayer->Set_InteractableObject(nullptr);
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	CGameObject* pCollided = nullptr;
	_bool bCollided = pGameInstance->Collision_with_Group(CCollision_Manager::COLLISION_PLAYER, Get_Collider(CCollider::AIM_OBJECT), CCollider::AIM_DAMAGE_INPUT, pCollided);

	RELEASE_INSTANCE(CGameInstance);

	if (bCollided && !m_bDidInteract)
	{
		pPlayer->Set_InteractableObject(this);
		return true;
	}
	else
	{
		if (pPlayer->Get_InteractableObject() == this)
			pPlayer->Set_InteractableObject(nullptr);

		return false;
	}
}

void CSword::Interact()
{
	m_bDidInteract = true;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	if (pPlayer)
		pPlayer->Set_AchieveState(ITEMID::ITEM_SWORD);

	m_bShouldDestroy = true;

	RELEASE_INSTANCE(CGameInstance);
}

void CSword::Spawn_InteractButton()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
	vPosition.x += 0.3f;
	vPosition.y += 0.3f;

	/* World to View */
	_matrix mViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	XMStoreFloat4(&vPosition, XMVector4Transform(XMLoadFloat4(&vPosition), mViewMatrix));

	/* View to Projection */
	_matrix mProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);
	XMStoreFloat4(&vPosition, XMVector4Transform(XMLoadFloat4(&vPosition), mProjMatrix));

	vPosition.x /= vPosition.w * 2;
	vPosition.y /= vPosition.w * 2;

	/* Projection to Screen */
	_float2 vScreenPosition;
	vScreenPosition.x = vPosition.x * g_iWinSizeX + g_iWinSizeX * 0.5f;
	vScreenPosition.y = -vPosition.y * g_iWinSizeY + g_iWinSizeY * 0.5f;

	if (m_pInteractButton)
	{
		m_pInteractButton->Set_PositionX(vScreenPosition.x);
		m_pInteractButton->Set_PositionY(vScreenPosition.y);
	}
	else
	{
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 120;
		tUIDesc.m_fSizeY = 44;
		tUIDesc.m_fX = vScreenPosition.x;
		tUIDesc.m_fY = vScreenPosition.y;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND_FADE_IN;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_InteractButton_Take"));

		pGameInstance->Add_GameObject_Out(TEXT("UI_InteractButton"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pInteractButton, &tUIDesc);
	}

	RELEASE_INSTANCE(CGameInstance);
}

CSword* CSword::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSword* pInstance = new CSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSword::Clone(void * pArg)
{
	CSword* pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
	__super::Free();

	if (m_pInteractButton)
	{
		m_pInteractButton->Set_ShouldDestroy(true);
		m_pInteractButton = nullptr;
	}
}