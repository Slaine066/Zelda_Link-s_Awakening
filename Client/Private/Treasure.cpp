#include "stdafx.h"

#include "Treasure.h"
#include "GameInstance.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerAchieveState.h"
#include "Item.h"
#include "UI.h"

CTreasure::CTreasure(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractableObject(pDevice, pContext)
{
}

CTreasure::CTreasure(const CTreasure & rhs)
	: CInteractableObject(rhs)
{
}

HRESULT CTreasure::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTreasure::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eInteractableType = INTERACTABLE_TREASURE;
	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_CLOSE_IDLE);

	return S_OK;
}

_uint CTreasure::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);
	
	if (iEvent == OBJ_STOP)
		return iEvent;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	return S_OK;
}

_uint CTreasure::Late_Tick(_float fTimeDelta)
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

	/* If Player has already Interacted and Treasure Chest is opened. */
	if (!m_bItemDropped && m_bDidInteract && m_pModelCom->Get_CurrentAnimIndex() == ANIMID::ANIM_OPEN_IDLE)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
		if (pPlayer)
		{
			switch (pGameInstance->Get_CurrentLevelIndex())
			{
			case LEVEL::LEVEL_FIELD:
				pPlayer->Set_AchieveState(ITEMID::ITEM_ROCFEATHER);
				break;
			case LEVEL::LEVEL_MORIBLINCAVE:
				pPlayer->Set_AchieveState(ITEMID::ITEM_OCARINA);
				break;
			}
		}

		m_bItemDropped = true;

		RELEASE_INSTANCE(CGameInstance);
	}

	Reset_Animation();

	return S_OK;
}

HRESULT CTreasure::Render()
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

HRESULT CTreasure::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;
	
	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vPosition = _float3(0.f, .5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTreasure"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(1.4f, 1.f, 1.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTreasureBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasure::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CTreasure::CanInteract()
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

	if (bCollided && m_pModelCom->Get_CurrentAnimIndex() == ANIM_CLOSE_IDLE && !m_bDidInteract)
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

void CTreasure::Interact()
{	
	/* Open Treasure if Player has Key */
	if (true /* TODO: Check if Player has Key */)
	{
		m_pModelCom->Set_CurrentAnimIndex(ANIM_OPEN);
		m_bDidInteract = true;
	}
	else
	{
		m_pModelCom->Set_CurrentAnimIndex(ANIM_OPEN_FAIL);
		m_bDidInteract = true;
	}
}

void CTreasure::Spawn_InteractButton()
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
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_InteractButton_Open"));

		pGameInstance->Add_GameObject_Out(TEXT("UI_InteractButton"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pInteractButton, &tUIDesc);
	}

	RELEASE_INSTANCE(CGameInstance);
}

_bool CTreasure::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_APPEAR:
	case ANIM_CLOSE:
	case ANIM_OPEN_FAIL:
	case ANIM_OPEN:
		return false;
		break;
	case ANIM_CLOSE_IDLE:
	case ANIM_OPEN_IDLE:
		return true;
		break;
	}
}

void CTreasure::Reset_Animation()
{
	if (m_bIsAnimationFinished)
	{
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case ANIM_OPEN:
			m_pModelCom->Set_CurrentAnimIndex(ANIM_OPEN_IDLE);
			break;
		case ANIM_OPEN_FAIL:
			m_pModelCom->Set_CurrentAnimIndex(ANIM_CLOSE_IDLE);
			m_bDidInteract = false;
			break;
		case ANIM_CLOSE:
			m_pModelCom->Set_CurrentAnimIndex(ANIM_CLOSE_IDLE);
			break;
		}
	}
}

CTreasure* CTreasure::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTreasure* pInstance = new CTreasure(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CTreasure"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTreasure::Clone(void * pArg)
{
	CTreasure* pInstance = new CTreasure(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CTreasure"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTreasure::Free()
{
	__super::Free();
}