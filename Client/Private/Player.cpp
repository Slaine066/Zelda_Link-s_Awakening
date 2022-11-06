#include "stdafx.h"

#include "Player.h"
#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CActor(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, .4f);
	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, .4f);
	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, .4f);

	m_pModelCom->Set_CurrentAnimIndex(ANIM_IDLE);

	return S_OK;
}

_uint CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Handle_Input();				// Change STATE based on Input and CurrentState.
	Execute_State(fTimeDelta);	// Execute Action based on STATE.

	m_bIsAnimationFinished = false;
	m_pModelCom->Play_Animation(fTimeDelta, m_bIsAnimationFinished, Is_AnimationLoop(m_pModelCom->Get_CurrentAnimIndex()));

	Reset_State();				// Change STATE when Animation ends.

	return OBJ_NOEVENT;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == MESH_SWORD_B || i == MESH_SWORD_B_HANDLE|| i == MESH_SHIELD_B|| i == MESH_SHIELD_B_MIRROR|| i == MESH_MAGIC_ROD ||
			i == MESH_HOOKSHOT || i == MESH_OCARINA || i == MESH_SHOVEL || i == MESH_FLIPPERS)
			continue;

		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Ready_Components(void* pArg)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, m_tModelDesc.wcModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
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

void CPlayer::Handle_Input()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	switch (m_eCurrentState)
	{
	case STATE_IDLE:
		if (pGameInstance->Key_Down('Z'))
		{
			m_eCurrentState = STATE_ATTACKING;
			m_pModelCom->Set_CurrentAnimIndex(ANIM_SLASH);
		}
		else if (Move())
		{
			m_eCurrentState = STATE_MOVING;
			m_pModelCom->Set_CurrentAnimIndex(ANIM_RUN);
		}
		else
			m_pModelCom->Set_CurrentAnimIndex(ANIM_IDLE);
		break;
	case STATE_MOVING:
		if (pGameInstance->Key_Down('Z'))
		{
			m_eCurrentState = STATE_ATTACKING;
			m_pModelCom->Set_CurrentAnimIndex(ANIM_SLASH);
		}
		else if (Move())
		{
			m_eCurrentState = STATE_MOVING;
			m_pModelCom->Set_CurrentAnimIndex(ANIM_RUN);
		}
		else
		{
			m_eCurrentState = STATE_IDLE;
			m_pModelCom->Set_CurrentAnimIndex(ANIM_IDLE);
		}
		break;
	case STATE_ATTACKING:
		break;
	}
}

void CPlayer::Execute_State(_float fTimeDelta)
{
	switch (m_eCurrentState)
	{
	case STATE_MOVING:
		switch (m_eCurrentDir)
		{
		case DIR_STRAIGHT_LEFT:
			m_pTransformCom->Go_Straight_Left(fTimeDelta);
			break;
		case DIR_STRAIGHT_RIGHT:
			m_pTransformCom->Go_Straight_Right(fTimeDelta);
			break;
		case DIR_BACKWARD_LEFT:
			m_pTransformCom->Go_Backward_Left(fTimeDelta);
			break;
		case DIR_BACKWARD_RIGHT:
			m_pTransformCom->Go_Backward_Right(fTimeDelta);
			break;
		case DIR_STRAIGHT:
			m_pTransformCom->Go_Straight(fTimeDelta);
			break;
		case DIR_BACKWARD:
			m_pTransformCom->Go_Backward(fTimeDelta);
			break;
		case DIR_LEFT:
			m_pTransformCom->Go_Left(fTimeDelta);
			break;
		case DIR_RIGHT:
			m_pTransformCom->Go_Right(fTimeDelta);
			break;
		}
	}
}

void CPlayer::Reset_State()
{
	if (m_bIsAnimationFinished)
	{
		switch ((ANIMID)m_pModelCom->Get_CurrentAnimIndex())
		{
		case ANIM_SLASH:
			m_eCurrentState = STATE_IDLE;
		}
	}
}

_bool CPlayer::Is_AnimationLoop(_uint eAnimId)
{
	switch ((ANIMID)eAnimId)
	{
	case ANIM_CARRY:
	case ANIM_IDLE:
	case ANIM_IDLE_BARE:
	case ANIM_IDLE_CARRY:
	case ANIM_ITEM_GET_LOOP:
	case ANIM_LADDER_DOWN:
	case ANIM_LADDER_UP:
	case ANIM_LADDER_WAIT:
	case ANIM_RUN:
	case ANIM_RUN_BARE:
	case ANIM_SHIELD_HOLD_BACK:
	case ANIM_SHIELD_HOLD_FRONT:
	case ANIM_SHIELD_HOLD_LEFT:
	case ANIM_SHIELD_HOLD_RIGHT:
	case ANIM_SHIELD_LOOP:
	case ANIM_SLASH_HOLD_BACK:
	case ANIM_SLASH_HOLD_FRONT:
	case ANIM_SLASH_HOLD_LEFT:
	case ANIM_SLASH_HOLD_LOOP:
	case ANIM_SLASH_HOLD_RIGHT:
	case ANIM_TALK:
	case ANIM_WALK:
	case ANIM_WALK_BARE:
		return true;
	case ANIM_BOW_END:
	case ANIM_BOW_START:
	case ANIM_CARRY_FAIL:
	case ANIM_DEATH:
	case ANIM_DAMAGE_AIR:
	case ANIM_DAMAGE_BACK:
	case ANIM_DAMAGE_FRONT:
	case ANIM_FALL_FROM_TOP:
	case ANIM_FALL_HOLE:
	case ANIM_ITEM_CARRY:
	case ANIM_ITEM_GET_END:
	case ANIM_ITEM_GET_START:
	case ANIM_ITEM_PUT:
	case ANIM_JUMP:
	case ANIM_JUMP_SLASH:
	case ANIM_KEY_OPEN:
	case ANIM_LADDER_DOWN_START:
	case ANIM_LADDER_UP_END:
	case ANIM_LADDER_UP_START:
	case ANIM_LAND:
	case ANIM_REVODOOR_IN:
	case ANIM_ROD_END:
	case ANIM_ROD_START:
	case ANIM_SHIELD_END:
	case ANIM_SHIELD_START:
	case ANIM_SLASH:
	case ANIM_SLASH_HOLD_END:
	case ANIM_SLASH_HOLD_START:
	case ANIM_THROW:
	case ANIM_WARP_END:
	case ANIM_WARP_START:
		return false;
	}
}

_bool CPlayer::Move()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_LEFT))
	{
		m_eCurrentDir = DIR_STRAIGHT_LEFT;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_UP) && pGameInstance->Key_Pressing(VK_RIGHT))
	{
		m_eCurrentDir = DIR_STRAIGHT_RIGHT;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_LEFT))
	{
		m_eCurrentDir = DIR_BACKWARD_LEFT;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_DOWN) && pGameInstance->Key_Pressing(VK_RIGHT))
	{
		m_eCurrentDir = DIR_BACKWARD_RIGHT;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_LEFT))
	{
		m_eCurrentDir = DIR_LEFT;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_RIGHT))
	{
		m_eCurrentDir = DIR_RIGHT;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_DOWN))
	{
		m_eCurrentDir = DIR_BACKWARD;
		return true;
	}
	else if (pGameInstance->Key_Pressing(VK_UP))
	{
		m_eCurrentDir = DIR_STRAIGHT;
		return true;
	}

	return false;
}

CPlayer* CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
}