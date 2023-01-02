#include "stdafx.h"

#include "Marin.h"
#include "GameInstance.h"
#include "MarinState.h"
#include "MarinIdleState.h"
#include "UI_Manager.h"
#include "UI.h"
#include "UI_Chat.h"

using namespace Marin;

CMarin::CMarin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNpc(pDevice, pContext)
{
}

CMarin::CMarin(const CMarin & rhs)
	: CNpc(rhs)
{
}

_bool CMarin::CanInteract()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	if (!pPlayer)
		return false;

	if (m_bDidInteract)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	CGameObject* pCollided = nullptr;
	_bool bCollided = pGameInstance->Collision_with_Group(CCollision_Manager::COLLISION_PLAYER, Get_Collider(CCollider::AIM_OBJECT), CCollider::AIM_DAMAGE_INPUT, pCollided);

	RELEASE_INSTANCE(CGameInstance);

	if (bCollided && !m_bDidInteract)
	{
		pPlayer->Set_Npc(this);
		return true;
	}
	else
	{
		if (pPlayer->Get_Npc() == this)
			pPlayer->Set_Npc(nullptr);

		return false;
	}
}

void CMarin::Interact()
{
	m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_TALK);

	if (m_pCurrentChat)
	{
		if (m_pCurrentChat->Get_ChatIndex() >= m_pCurrentChat->Get_ChatCount() - 1)
		{
			m_pModelCom->Set_CurrentAnimIndex(ANIMID::ANIM_WAIT);
			
			m_pCurrentChat->Set_ShouldDestroy(true);
			m_pCurrentChat = nullptr;

			m_bDidInteract = true;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
			if (!pPlayer)
				return;

			pPlayer->Set_Npc(nullptr);
			RELEASE_INSTANCE(CGameInstance);
		}
		else
			m_pCurrentChat->Increase_ChatIndex();
	}
	else
	{
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 1161;
		tUIDesc.m_fSizeY = 250;
		tUIDesc.m_fX = g_iWinSizeX / 2;
		tUIDesc.m_fY = g_iWinSizeY - 175;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Chat_Marin_Line_1"));

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Add_GameObject_Out(TEXT("UI_Chat_Marin"), TEXT("Prototype_GameObject_UI_Chat"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pCurrentChat, &tUIDesc);
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CMarin::Compute_ChatLine()
{
	/* When Game starts ChatLine should be 1. */
}

void CMarin::Spawn_InteractButton()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
	vPosition.y -= 0.5f;

	/* World to View */
	_matrix mViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	XMStoreFloat4(&vPosition, XMVector3TransformCoord(XMLoadFloat4(&vPosition), mViewMatrix));

	/* View to Projection */
	_matrix mProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);
	XMStoreFloat4(&vPosition, XMVector3TransformCoord(XMLoadFloat4(&vPosition), mProjMatrix));

	/* Projection to Screen */
	_float2 fScreenPosition;
	fScreenPosition.x = vPosition.x * g_iWinSizeX + g_iWinSizeX * 0.5f;
	fScreenPosition.y = -vPosition.y * g_iWinSizeY + g_iWinSizeY * 0.5f;

	if (m_pInteractButton)
	{
		m_pInteractButton->Set_PositionX(fScreenPosition.x);
		m_pInteractButton->Set_PositionY(fScreenPosition.y);
	}
	else
	{
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 120;
		tUIDesc.m_fSizeY = 44;
		tUIDesc.m_fX = fScreenPosition.x;
		tUIDesc.m_fY = fScreenPosition.y;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_InteractButton_Talk"));

		pGameInstance->Add_GameObject_Out(TEXT("UI_InteractButton"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pInteractButton, &tUIDesc);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMarin::Initialize_Prototype()
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

HRESULT CMarin::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRadius = .25f;

	CMarinState* pState = new CIdleState();
	m_pMarinState = m_pMarinState->ChangeState(this, m_pMarinState, pState);

	m_iChatLineIndex = CUI_Manager::Get_Instance()->Get_MarinChatLine();

	return S_OK;
}

_uint CMarin::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	AI_Behavior();
	TickState(fTimeDelta);

	return OBJ_NOEVENT;
}

_uint CMarin::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom && m_bIsInFrustum)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	LateTickState(fTimeDelta);

	return OBJ_NOEVENT;
}

HRESULT CMarin::Render()
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

HRESULT CMarin::Ready_Components(void * pArg)
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

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderMarin"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(.8f, 1.4f, .8f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderMarinBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMarin::SetUp_ShaderResources()
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

void CMarin::AI_Behavior()
{
	CMarinState* pNewState = m_pMarinState->AI_Behavior(this);
	if (pNewState)
		m_pMarinState = m_pMarinState->ChangeState(this, m_pMarinState, pNewState);
}

void CMarin::TickState(_float fTimeDelta)
{
	CMarinState* pNewState = m_pMarinState->Tick(this, fTimeDelta);
	if (pNewState)
		m_pMarinState = m_pMarinState->ChangeState(this, m_pMarinState, pNewState);
}

void CMarin::LateTickState(_float fTimeDelta)
{
	CMarinState* pNewState = m_pMarinState->LateTick(this, fTimeDelta);
	if (pNewState)
		m_pMarinState = m_pMarinState->ChangeState(this, m_pMarinState, pNewState);
}

_bool CMarin::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_TALK:
	case ANIM_WAIT:
	case ANIM_WALK:
		return true;
	}
}

CMarin * CMarin::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMarin* pInstance = new CMarin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CMarin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMarin::Clone(void * pArg)
{
	CMarin* pInstance = new CMarin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CMarin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMarin::Free()
{
	__super::Free();

	Safe_Delete(m_pMarinState);
}