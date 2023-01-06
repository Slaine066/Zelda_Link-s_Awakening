#include "stdafx.h"

#include "Camera_Dungeon.h"
#include "GameInstance.h"
#include "Player.h"
#include "Level_MoriblinCave.h"

CCamera_Dungeon::CCamera_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dungeon::CCamera_Dungeon(const CCamera_Dungeon & rhs)
	: CCamera(rhs)
{
}

void CCamera_Dungeon::Setup_DungeonCamera(_float3 vCurrentRoomPosition)
{
	m_vCurrentRoomPosition = vCurrentRoomPosition;
	m_pTransform->LookAt(XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f));
	m_pTransform->Attach_ToTarget(XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f), XMVectorSet(0.f, 3.5f, -2., 1.f)); /* Distance from Camera to Player */
}

HRESULT CCamera_Dungeon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dungeon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera_Dungeon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	switch (m_eCamMode)
	{
	case MODE_DUNGEON:
		Dungeon_Camera(fTimeDelta);
		break;
	case MODE_ZOOMIN:
		ZoomIn_Camera(fTimeDelta, m_vZoomPosition);
		break;
	case MODE_ZOOMOUT:
		ZoomOut_Camera(fTimeDelta);
		break;
	case MODE_SHAKING:
		Shaking_Camera(fTimeDelta);
		break;
	}

	if (FAILED(Bind_OnPipeLine()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

_uint CCamera_Dungeon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

HRESULT CCamera_Dungeon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Dungeon::Dungeon_Camera(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Dungeon Camera*/
	if (pGameInstance->Key_Up(VK_SHIFT))
	{
		m_pTransform->LookAt(XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f));
		m_pTransform->Attach_ToTarget(XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f), XMVectorSet(0.f, 3.5f, -2., 1.f));
	}
	else if (!pGameInstance->Key_Pressing(VK_SHIFT))
	{
		CLevel_MoriblinCave* pDungeonLevel = dynamic_cast<CLevel_MoriblinCave*>(pGameInstance->Get_CurrentLevel());
		if (!pDungeonLevel)
			return;

		_float4 pCurrentRoomPosition = pDungeonLevel->Get_CurrentRoom()->m_vRoomPosition;
		/* Player changed room, so Camera should move to the next Room. ("m_bMoving" = TRUE) */
		if (pDungeonLevel->Get_RoomChanged())
		{
			m_vCurrentRoomPosition = _float3(pCurrentRoomPosition.x, pCurrentRoomPosition.y, pCurrentRoomPosition.z);
			m_bMoving = true;
		}

		/* Compute Target Camera Position */
		_vector vTargetCameraPosition = XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f) + XMVectorSet(0.f, 3.5f, -2.f, 1.f);

		if (m_bMoving)
		{
			_float3 vStoredTarget;
			XMStoreFloat3(&vStoredTarget, vTargetCameraPosition);

			/* Check if the Camera arrived to "vTargetCameraPosition". (If yes, "m_bMoving" = FALSE) */
			_float4 vCameraPosition = pGameInstance->Get_CamPosition();
			if (XMVector4Equal(XMLoadFloat4(&vCameraPosition), XMVectorSetW(XMLoadFloat3(&vStoredTarget), 1.f)))
			{
				pDungeonLevel->Close_Door();

				m_bMoving = false;
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			/* If "m_bMoving", move to "vTargetCameraPosition". */
			if (m_pTransform->Go_TargetPosition(fTimeDelta, vStoredTarget, 0.f))
			{
				vCameraPosition = pGameInstance->Get_CamPosition();
				m_vCurrentRoomPosition = _float3(vCameraPosition.x, vCameraPosition.y - 3.5, vCameraPosition.z + 2);
			}
		}
		else
		{
			/* If not "m_bMoving", just keep same Camera Position and Look. */
			m_pTransform->LookAt(XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f));
			m_pTransform->Attach_ToTarget(XMVectorSetW(XMLoadFloat3(&m_vCurrentRoomPosition), 1.f), XMVectorSet(0.f, 3.5f, -2., 1.f));
		}	
	}
	/* Debug Camera*/
	else
	{
		/* Camera Inputs */
		if (pGameInstance->Key_Pressing('W') && pGameInstance->Key_Pressing(VK_SHIFT))
			m_pTransform->Move_Straight(fTimeDelta);
		if (pGameInstance->Key_Pressing('S') && pGameInstance->Key_Pressing(VK_SHIFT))
			m_pTransform->Move_Backward(fTimeDelta);
		if (pGameInstance->Key_Pressing('A') && pGameInstance->Key_Pressing(VK_SHIFT))
			m_pTransform->Move_Left(fTimeDelta);
		if (pGameInstance->Key_Pressing('D') && pGameInstance->Key_Pressing(VK_SHIFT))
			m_pTransform->Move_Right(fTimeDelta);

		_long MouseMove = 0;
		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X))
			m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);
		if (MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y))
			m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dungeon::ZoomIn_Camera(_float fTimeDelta, _float3 vZoomPosition)
{
	vZoomPosition.y += 2.f;
	vZoomPosition.z -= 1.f;

	m_pTransform->Go_TargetPosition(fTimeDelta / 5, vZoomPosition, 0.f);
}

void CCamera_Dungeon::ZoomOut_Camera(_float fTimeDelta)
{
	if (m_pTransform->Go_TargetPosition(fTimeDelta / 5, _float3(m_vCurrentRoomPosition.x, m_vCurrentRoomPosition.y + 3.5f, m_vCurrentRoomPosition.z - 2.f), 0.f))
	{
		XMStoreFloat3(&m_vCurrentRoomPosition, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
		m_vCurrentRoomPosition.y -= 3.5f;
		m_vCurrentRoomPosition.z += 2.f;

		m_eCamMode = MODE_DUNGEON;
	}
}

void CCamera_Dungeon::Shaking_Camera(_float fTimeDelta)
{
	
}

CCamera_Dungeon * CCamera_Dungeon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dungeon* pInstance = new CCamera_Dungeon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CCamera_Dungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dungeon::Clone(void* pArg)
{
	CCamera_Dungeon* pInstance = new CCamera_Dungeon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CCamera_Dungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dungeon::Free()
{
	__super::Free();
}