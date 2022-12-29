#include "stdafx.h"

#include "Effect.h"
#include "GameInstance.h"
#include "Actor.h"

/* 
Lerp Interpolation in C++

lerp(start, end, t) = start + t * (end - start);

float t = (m_fEffectTimer - 1.0f) / (m_fEffectTimerMax - 1.0f);
fNewScale = 1.0f + t * (2.0f - 1.0f);

(!~Ask https://chat.openai.com/chat for clarifications.)
*/

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs)
{

}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	switch (m_tEffectDesc.m_eEffectType)
	{
		case EFFECT_TYPE::EFFECT_SMOKE:
		{
			m_eShaderPass = VTXTEXPASS::VTXTEX_EFFECT_SMOKE;

			m_fEffectScale = .3f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			_float4 vPosition; 
			XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

			vPosition.y += m_pTransformCom->Get_Scale(CTransform::STATE::STATE_RIGHT) / 4;
			m_pTransformCom->Set_State(CTransform::STATE::STATE_TRANSLATION, XMLoadFloat4(&vPosition));

			break;
		}	
		case EFFECT_TYPE::EFFECT_SWORD_SLASH:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_SWORDSLASH;

			_vector vRight = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT);
			_vector vUp = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
			_vector vLook = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);

			m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, vRight);
			m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, vUp);
			m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, vLook);

			m_fEffectScale = .3f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			m_pTransformCom->Move_Straight(3.f);
			m_pTransformCom->Move_Up(2.1f);

			break;
		}
		case EFFECT_TYPE::EFFECT_HIT_RING:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_HITRING;

			m_fEffectScale = .2f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			/* Move it a bit far away to the Camera, cause it has some Z-fighting with VTXTEX_EFFECT_HIT_FLASH. */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			m_pTransformCom->Move_Backward(0.01f); 
			RELEASE_INSTANCE(CGameInstance);

			break;
		}
		case EFFECT_TYPE::EFFECT_HIT:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_HIT;

			m_fEffectScale = .15f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			/* Move it a bit closer to the Camera, cause it has some Z-fighting with VTXMODEL_HIT. */
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			m_pTransformCom->Move_Straight(0.01f);
			RELEASE_INSTANCE(CGameInstance);

			break;
		}
		case EFFECT_TYPE::EFFECT_HIT_FLASH:
		{
			m_eShaderPass = VTXTEXPASS::VTXTEX_EFFECT_HIT_FLASH;

			m_fEffectScale = .5f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			break;
		}
		case EFFECT_TYPE::EFFECT_GUARD_RING:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_GUARDRING;

			if (m_tEffectDesc.m_bIsPositionDynamic)
			{
				_float4 vPosition;
				XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));
				
				_float4 vOwnerPosition; 
				XMStoreFloat4(&vOwnerPosition, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION));
				vOwnerPosition.y = vPosition.y;

				_vector vLook = XMLoadFloat4(&vPosition) - XMLoadFloat4(&vOwnerPosition);
				vLook = XMVector4Normalize(vLook);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, vLook);

				_vector vUp = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
				_vector vRight = XMVector3Cross(vLook, vUp);
				vRight = XMVector4Normalize(vRight);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, vRight);

				vUp = XMVector3Cross(vLook, vRight);
				vUp = XMVector4Normalize(vUp);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, vUp);
			}
			else
			{
				/* Set Orientation related to Player and not to Bone. */
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

				/* Move Straight. */
				m_pTransformCom->Move_Straight(1.f);
			}

			/* Set Scale. */
			m_fEffectScale = .25f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			break;
		}
		case EFFECT_TYPE::EFFECT_GUARD:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_GUARD;

			if (m_tEffectDesc.m_bIsPositionDynamic)
			{
				_float4 vPosition;
				XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

				_float4 vOwnerPosition;
				XMStoreFloat4(&vOwnerPosition, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION));
				vOwnerPosition.y = vPosition.y;

				_vector vLook = XMLoadFloat4(&vPosition) - XMLoadFloat4(&vOwnerPosition);
				vLook = XMVector4Normalize(vLook);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, vLook);

				_vector vUp = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
				_vector vRight = XMVector3Cross(vLook, vUp);
				vRight = XMVector4Normalize(vRight);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, vRight);

				vUp = XMVector3Cross(vLook, vRight);
				vUp = XMVector4Normalize(vUp);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, vUp);
			}
			else
			{
				/* Set Orientation related to Player and not to Bone. */
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));

				/* Move Straight. */
				m_pTransformCom->Move_Straight(1.f);
			}

			/* Set Scale. */
			m_fEffectScale = .125f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			break;
		}
		case EFFECT_TYPE::EFFECT_SHOCKWAVE_RING:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_SHOCKWAVERING;

			if (m_tEffectDesc.m_bIsPositionDynamic)
			{
				_float4 vPosition;
				XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

				_float4 vOwnerPosition;
				XMStoreFloat4(&vOwnerPosition, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION));
				vOwnerPosition.y = vPosition.y - .3f;

				_vector vLook = XMLoadFloat4(&vPosition) - XMLoadFloat4(&vOwnerPosition);
				vLook = XMVector4Normalize(vLook);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, vLook);

				_vector vUp = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
				_vector vRight = XMVector3Cross(vLook, vUp);
				vRight = XMVector4Normalize(vRight);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, vRight);

				vUp = XMVector3Cross(vLook, vRight);
				vUp = XMVector4Normalize(vUp);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, vUp);
			}
			else
			{
				/* Set Orientation related to Player and not to Bone. */
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
			}

			/* Set Scale. */
			m_fEffectScale = .1f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			/* Move Straight.*/
			m_pTransformCom->Move_Straight(.19f);

			break;
		}
		case EFFECT_TYPE::EFFECT_SHOCKWAVE:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_SHOCKWAVE;

			if (m_tEffectDesc.m_bIsPositionDynamic)
			{
				_float4 vPosition;
				XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

				_float4 vOwnerPosition;
				XMStoreFloat4(&vOwnerPosition, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION));
				vOwnerPosition.y = vPosition.y - .3f;

				_vector vLook = XMLoadFloat4(&vPosition) - XMLoadFloat4(&vOwnerPosition);
				vLook = XMVector4Normalize(vLook);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, vLook);

				_vector vUp = m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP);
				_vector vRight = XMVector3Cross(vLook, vUp);
				vRight = XMVector4Normalize(vRight);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, vRight);

				vUp = XMVector3Cross(vLook, vRight);
				vUp = XMVector4Normalize(vUp);
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, vUp);
			}
			else
			{
				/* Set Orientation related to Player and not to Bone. */
				m_pTransformCom->Set_State(CTransform::STATE::STATE_RIGHT, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_RIGHT));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_UP, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_UP));
				m_pTransformCom->Set_State(CTransform::STATE::STATE_LOOK, m_tEffectDesc.m_pOwner->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
			}

			/* Set Scale. */
			m_fEffectScale = .25f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			/* Move Straight.*/
			m_pTransformCom->Move_Straight(.2f);

			break;
		}
		case EFFECT_TYPE::EFFECT_STAR:
		{
			m_eShaderModelPass = VTXMODELPASS::VTXMODEL_STAR;

			m_fEffectScale = .05f;
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, m_fEffectScale);
			m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, m_fEffectScale);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			RELEASE_INSTANCE(CGameInstance);

			Calculate_Angle();

			break;
		}
		case EFFECT_TYPE::EFFECT_TREASURE:
			break;
		case EFFECT_TYPE::EFFECT_GET_ITEM:
			break;
		case EFFECT_TYPE::EFFECT_BOMB_EXPLOSION:
			break;
	}	

	return S_OK;
}

_uint CEffect::Tick(_float fTimeDelta)
{
	switch (m_tEffectDesc.m_eEffectType)
	{
		case EFFECT_TYPE::EFFECT_SMOKE:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
				
				/* Decrease Scale based on Time. */
				_float fInterpFactor = m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;

				_float fScale = m_fEffectScale + fInterpFactor * (0.f - m_fEffectScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_SWORD_SLASH:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_HIT_RING:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
				
				/* Increase Scale based on Time. */
				_float fInterpFactor = m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;

				_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 2 - m_fEffectScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_HIT:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));

				/* Increase Scale based on Time. */
				if (m_fEffectTimer < m_tEffectDesc.m_fEffectLifespan / 2)
				{
					_float fInterpFactor = m_fEffectTimer / (m_tEffectDesc.m_fEffectLifespan / 2);

					_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 2 - m_fEffectScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				/* Decrease Scale based on Time. */
				else
				{
					_float fInterpFactor = (m_fEffectTimer - (m_tEffectDesc.m_fEffectLifespan / 2)) / (m_tEffectDesc.m_fEffectLifespan - (m_tEffectDesc.m_fEffectLifespan / 2));

					_float fScale = m_fEffectScale * 2 + fInterpFactor * (m_fEffectScale / 4 - m_fEffectScale * 2);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				
				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_HIT_FLASH:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));

				/* Increase Scale based on Time. */
				_float fInterpFactor = m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;

				_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 2 - m_fEffectScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				RELEASE_INSTANCE(CGameInstance);

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_GUARD_RING:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				/* Increase Scale based on Time. */
				_float fInterpFactor = m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;

				_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 2 - m_fEffectScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_GUARD:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				/* Increase Scale based on Time. */
				if (m_fEffectTimer < m_tEffectDesc.m_fEffectLifespan / 2)
				{
					_float fInterpFactor = m_fEffectTimer / (m_tEffectDesc.m_fEffectLifespan / 2);

					_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 2 - m_fEffectScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				/* Decrease Scale based on Time. */
				else
				{
					_float fInterpFactor = (m_fEffectTimer - (m_tEffectDesc.m_fEffectLifespan / 2)) / (m_tEffectDesc.m_fEffectLifespan - (m_tEffectDesc.m_fEffectLifespan / 2));

					_float fScale = m_fEffectScale * 2 + fInterpFactor * (m_fEffectScale / 4 - m_fEffectScale * 2);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_SHOCKWAVE_RING:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				_float fInterpFactor = m_fEffectTimer / m_tEffectDesc.m_fEffectLifespan;

				_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 3 - m_fEffectScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
				m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_SHOCKWAVE:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				/* Increase Scale based on Time. */
				if (m_fEffectTimer < m_tEffectDesc.m_fEffectLifespan / 2)
				{
					_float fInterpFactor = m_fEffectTimer / (m_tEffectDesc.m_fEffectLifespan / 2);

					_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 2 - m_fEffectScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				/* Decrease Scale based on Time. */
				else
				{
					_float fInterpFactor = (m_fEffectTimer - (m_tEffectDesc.m_fEffectLifespan / 2)) / (m_tEffectDesc.m_fEffectLifespan - (m_tEffectDesc.m_fEffectLifespan / 2));

					_float fScale = m_fEffectScale * 2 + fInterpFactor * (m_fEffectScale / 4 - m_fEffectScale * 2);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}

				m_fEffectTimer += fTimeDelta;
			}

			break;
		}
		case EFFECT_TYPE::EFFECT_STAR:
		{
			if (m_fEffectTimer >= m_tEffectDesc.m_fEffectLifespan)
				return OBJ_DESTROY;
			else
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				/* Rotate Local. */
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK), fTimeDelta);

				/* Rotate around a Point. */
				Rotate_AroundCenter(fTimeDelta);

				/* Increase Scale based on Time. */
				if (m_fEffectTimer < m_tEffectDesc.m_fEffectLifespan / 8)
				{
					_float fInterpFactor = m_fEffectTimer / (m_tEffectDesc.m_fEffectLifespan / 8);

					_float fScale = m_fEffectScale + fInterpFactor * (m_fEffectScale * 3 - m_fEffectScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				/* Decrease Scale based on Time. */
				else if (m_fEffectTimer > m_tEffectDesc.m_fEffectLifespan - (m_tEffectDesc.m_fEffectLifespan / 8))
				{
					_float fInterpFactor = (m_fEffectTimer - (m_tEffectDesc.m_fEffectLifespan - m_tEffectDesc.m_fEffectLifespan / 8)) / (m_tEffectDesc.m_fEffectLifespan - (m_tEffectDesc.m_fEffectLifespan - m_tEffectDesc.m_fEffectLifespan / 8));

					_float fScale = m_fEffectScale * 3 + fInterpFactor * (0 - m_fEffectScale * 3);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, fScale);
					m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, fScale);
				}
				else
				{
					
				}

				m_fEffectTimer += fTimeDelta;

				RELEASE_INSTANCE(CGameInstance);
			}

			/* TODO: .. */

			break;
		}
		case EFFECT_TYPE::EFFECT_TREASURE:
			break;
		case EFFECT_TYPE::EFFECT_GET_ITEM:
			break;
		case EFFECT_TYPE::EFFECT_BOMB_EXPLOSION:
			break;
	}

	return OBJ_NOEVENT;
}

_uint CEffect::Late_Tick(_float fTimeDelta)
{
	if (m_pRendererCom)
	{
		if (m_tEffectDesc.m_eEffectType == EFFECT_TYPE::EFFECT_STAR)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHALIGHT, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		
		Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	}

	return OBJ_NOEVENT;
}

HRESULT CEffect::Render()
{
	if (m_tEffectDesc.m_eEffectType == EFFECT_TYPE::EFFECT_HIT)
	{
		if (m_fEffectTimer <= (m_tEffectDesc.m_fEffectLifespan / 100 * 15))
			return S_OK;
	}

	if (Is_ModelEffect())
	{
		if (!m_pShaderCom || !m_pModelCom)
			return E_FAIL;
	}
	else if(!m_pShaderCom || !m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	if (Is_ModelEffect())
	{
		_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->SetUp_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom->SetUp_Material(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR);

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderModelPass)))
				return E_FAIL;
		}
	}
	else
	{
		m_pShaderCom->Begin(m_eShaderPass);
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CEffect::Ready_Components(void * pArg)
{
	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	memcpy(&m_tEffectDesc, (EFFECTDESC*)pArg, sizeof(EFFECTDESC));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.vInitialWorldMatrix = m_tEffectDesc.m_WorldMatrix;
	TransformDesc.fSpeedPerSec = 0.1f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (Is_ModelEffect())
	{	
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, Get_ModelPrototypeId(), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		/* For.Com_Texture */
		if (!Is_ModelEffect())
		{			
			if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, Get_TextureName(), (CComponent**)&m_pTextureCom)))
				return E_FAIL;
		}

		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect::SetUp_ShaderResources()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (!Is_ModelEffect())
	{
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectTimer", &m_fEffectTimer, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_EffectLifespan", &m_tEffectDesc.m_fEffectLifespan, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_bool CEffect::Is_ModelEffect()
{
	_bool bIsModel = false;
	switch (m_tEffectDesc.m_eEffectType)
	{
	case EFFECT_TYPE::EFFECT_SMOKE:
	case EFFECT_TYPE::EFFECT_HIT_FLASH:
		bIsModel = false;
		break;

	case EFFECT_TYPE::EFFECT_SWORD_SLASH:
	case EFFECT_TYPE::EFFECT_HIT_RING:
	case EFFECT_TYPE::EFFECT_HIT:
	case EFFECT_TYPE::EFFECT_GUARD_RING:
	case EFFECT_TYPE::EFFECT_GUARD:
	case EFFECT_TYPE::EFFECT_SHOCKWAVE_RING:
	case EFFECT_TYPE::EFFECT_SHOCKWAVE:
	case EFFECT_TYPE::EFFECT_STAR:
		bIsModel = true;
		break;
	}

	return bIsModel;
}

_tchar * CEffect::Get_TextureName()
{
	switch (m_tEffectDesc.m_eEffectType)
	{
	case EFFECT_TYPE::EFFECT_SMOKE:
		return TEXT("Prototype_Component_Texture_Smoke");
	case EFFECT_TYPE::EFFECT_HIT_FLASH:
		return TEXT("Prototype_Component_Texture_Hit_Flash");
	}
}

_tchar * CEffect::Get_ModelPrototypeId()
{
	switch (m_tEffectDesc.m_eEffectType)
	{
	case EFFECT_TYPE::EFFECT_SWORD_SLASH:
		return TEXT("Prototype_Component_Model_SwordSlash");
	case EFFECT_TYPE::EFFECT_HIT_RING:
	case EFFECT_TYPE::EFFECT_GUARD_RING:
		return TEXT("Prototype_Component_Model_HitRing");
	case EFFECT_TYPE::EFFECT_HIT:
		return TEXT("Prototype_Component_Model_HitFlash");
	case EFFECT_TYPE::EFFECT_GUARD:
		return TEXT("Prototype_Component_Model_GuardFlash");
	case EFFECT_TYPE::EFFECT_SHOCKWAVE_RING:
		return TEXT("Prototype_Component_Model_ShockwaveRing");
	case EFFECT_TYPE::EFFECT_SHOCKWAVE:
		return TEXT("Prototype_Component_Model_Shockwave");
	case EFFECT_TYPE::EFFECT_STAR:
		return TEXT("Prototype_Component_Model_Star");
	}
}

void CEffect::Calculate_Angle()
{
	_float4 vOrigin; 
	XMStoreFloat4(&vOrigin, XMVectorSet(1.f, 0.f, 0.f, 0.f));

	CHierarchyNode* m_pSocket = m_tEffectDesc.m_pOwner->Get_Model()->Get_BonePtr("jaw");
	_matrix SocketMatrix = m_pSocket->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_tEffectDesc.m_pOwner->Get_Model()->Get_PivotFloat4x4()) * XMLoadFloat4x4(&m_tEffectDesc.m_pOwner->Get_Transform()->Get_World4x4());
	_matrix mOffset = XMMatrixTranslation(0.f, .3f, 0.f);
	_matrix mWorldMatrix = SocketMatrix * mOffset;

	m_vCenterPosition;
	XMStoreFloat4(&m_vCenterPosition, mWorldMatrix.r[3]);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	_float4 vDirection;
	XMStoreFloat4(&vDirection, XMLoadFloat4(&vPosition) - XMLoadFloat4(&m_vCenterPosition));
	XMStoreFloat4(&vDirection, XMVector4Normalize(XMLoadFloat4(&vDirection)));

	_float fDot = XMVectorGetX(XMVector3Dot(XMLoadFloat4(&vDirection), XMLoadFloat4(&vOrigin)));
	_float fAngle = XMConvertToDegrees(acosf(fDot));

	_float4 vCross; 
	XMStoreFloat4(&vCross, XMVector3Cross(XMLoadFloat4(&vOrigin), XMLoadFloat4(&vDirection)));

	if (vCross.y > 0)
		fAngle = 360 - fAngle;

	m_fAngle = fAngle;
}

void CEffect::Rotate_AroundCenter(_float fTimeDelta)
{
	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE::STATE_TRANSLATION));

	_float fDistance = .2f;

	m_fAngle += 1.5f;
	if (m_fAngle >= 360.f)
		m_fAngle = 0.f;

	vPosition.x = m_vCenterPosition.x + cosf(XMConvertToRadians(m_fAngle)) * fDistance - sin(XMConvertToRadians(m_fAngle)) * fDistance;
	vPosition.z = m_vCenterPosition.z + sin(XMConvertToRadians(m_fAngle)) * fDistance + cos(XMConvertToRadians(m_fAngle)) * fDistance;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPosition));
}

CEffect * CEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect* pInstance = new CEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect::Clone(void * pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}