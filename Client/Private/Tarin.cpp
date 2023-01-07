#include "stdafx.h"

#include "Tarin.h"
#include "GameInstance.h"
#include "TarinState.h"
#include "TarinIdleState.h"
#include "UI_Manager.h"
#include "UI.h"
#include "UI_Chat.h"

using namespace Tarin;

CTarin::CTarin(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNpc(pDevice, pContext)
{
}

CTarin::CTarin(const CTarin & rhs)
	: CNpc(rhs)
{
}

_bool CTarin::CanInteract()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	if (!pPlayer)
		return false;

	if (m_bDidInteract)
	{
		if (pPlayer->Get_Npc() == this)
			pPlayer->Set_Npc(nullptr);

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

void CTarin::Interact()
{
	/* If a Chat is already showing */
	if (m_pCurrentChat)
	{
		/* If Chat is finished */
		if (m_pCurrentChat->Get_ChatIndex() >= m_pCurrentChat->Get_ChatCount() - 1)
		{
			m_pCurrentChat->Set_ShouldDestroy(true);
			m_pCurrentChat = nullptr;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
			if (!pPlayer)
				return;
			
			m_bDidInteract = true;
			Process_ChatLine();

			RELEASE_INSTANCE(CGameInstance);
		}
		/* Go to next Chat */
		else
			m_pCurrentChat->Increase_ChatIndex();
	}
	/* Create Chat */
	else
	{
		CUI::UIDESC tUIDesc;
		ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));
		tUIDesc.m_fSizeX = 1161;
		tUIDesc.m_fSizeY = 250;
		tUIDesc.m_fX = g_iWinSizeX / 2;
		tUIDesc.m_fY = g_iWinSizeY - 175;
		tUIDesc.m_ePass = VTXTEXPASS::VTXTEX_UI_BLEND;

		/* Load correct Chat Line. */
		_tchar wcTextureName[MAX_PATH] = TEXT("Prototype_Component_Texture_Chat_Tarin_Line_");
		_tchar wcTextureId[MAX_PATH];
		swprintf_s(wcTextureId, L"%d", m_iChatLineIndex);
		wcscat_s(wcTextureName, MAX_PATH, wcTextureId); 
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, wcTextureName);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Add_GameObject_Out(TEXT("UI_Chat_Tarin"), TEXT("Prototype_GameObject_UI_Chat"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pCurrentChat, &tUIDesc);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CTarin::Compute_ChatLine()
{
	m_iChatLineIndex = CUI_Manager::Get_Instance()->Get_TarinChatLine();
	switch (m_iChatLineIndex)
	{
		case 1:
		{
			m_bDidInteract = false;
			break;
		}
		case 2:
		{
			m_bDidInteract = false;
			break;
		}
		case 3:
		{
			m_bDidInteract = true;
			break;
		}
	}
}

void CTarin::Process_ChatLine()
{
	switch (m_iChatLineIndex)
	{
		case 1:
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
			if (!pPlayer)
				return;

			pPlayer->Set_AchieveState(ITEMID::ITEM_SHIELD);
			RELEASE_INSTANCE(CGameInstance);

			break;
		}
		case 2:
		{
			break;
		}
		case 3:
		{
			break;
		}
	}

	CUI_Manager::Get_Instance()->Increase_TarinChatLine();
}

void CTarin::Spawn_InteractButton()
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
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_InteractButton_Talk"));

		pGameInstance->Add_GameObject_Out(TEXT("UI_InteractButton"), TEXT("Prototype_GameObject_UI"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_UI"), (CGameObject*&)m_pInteractButton, &tUIDesc);
	}

	RELEASE_INSTANCE(CGameInstance);
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

	m_iChatLineIndex = CUI_Manager::Get_Instance()->Get_TarinChatLine();

	return S_OK;
}

_uint CTarin::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	AI_Behavior();
	TickState(fTimeDelta);

	Compute_ChatLine();

	return OBJ_NOEVENT;
}

_uint CTarin::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	if (m_pRendererCom && m_bIsInFrustum)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}

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
		if (i == MESHID::MESH_BACKBACK || i == MESHID::MESH_BACKBACK_STRAPS || i == MESHID::MESH_BANANA)
			continue;

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

HRESULT CTarin::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == MESHID::MESH_BACKBACK || i == MESHID::MESH_BACKBACK_STRAPS || i == MESHID::MESH_BANANA)
			continue;

		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
		m_pModelCom->SetUp_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, VTXANIMMODELPASS::VTXANIMMODEL_SHADOW)))
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

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_OBJECT;
	ColliderDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	m_vCollidersCom.resize(2); // Numbers of Colliders needed for this Object

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTarin"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_BLOCK;
	ColliderDesc.vScale = _float3(.8f, 1.4f, .8f);
	ColliderDesc.vPosition = _float3(0.f, 0.75f, 0.f);
	ColliderDesc.m_bIsAttachedToBone = false;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_ColliderTarinBlock"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_vCollidersCom[1], &ColliderDesc)))
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

HRESULT CTarin::SetUp_ShadowShaderResources()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* World Matrix*/
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	_float4 vEye = vPosition;
	vEye.x -= 15.f;
	vEye.y += 25.f;
	vEye.z -= 15.f;

	CLight_Manager::Get_Instance()->Set_ShadowLightViewMat(vEye, vPosition);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &CLight_Manager::Get_Instance()->Get_ShadowLightViewMatrix(), sizeof(_float4x4))))
		return E_FAIL;

	/* Light Proj Matrix */
	_float4x4 matProjMatrixTP = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &matProjMatrixTP, sizeof(_float4x4))))
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