#include "stdafx.h"

#include "Camera_Dungeon.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Dungeon::CCamera_Dungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dungeon::CCamera_Dungeon(const CCamera_Dungeon & rhs)
	: CCamera(rhs)
{
}

void CCamera_Dungeon::Setup_DungeonCamera()
{
	m_vCurrentCameraPosition = Compute_DungeonCamera();
	m_pTransform->LookAt(XMVectorSetW(XMLoadFloat3(&m_vCurrentCameraPosition), 1.f));
	m_pTransform->Attach_ToTarget(XMVectorSetW(XMLoadFloat3(&m_vCurrentCameraPosition), 1.f), XMVectorSet(0.f, 3.5f, -2., 1.f)); /* Distance from Camera to Player */
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

_float3 CCamera_Dungeon::Compute_DungeonCamera()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vCameraPosition;
	_float fPreviousDistance = 999.f;

	for (auto& vPosition : m_CameraPositions)
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_NextLevelIndex(), TEXT("Layer_Player"));
		_vector vPlayerPosition = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
		vPlayerPosition = XMVectorSetW(vPlayerPosition, 1.0f);

		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPosition) - vPlayerPosition));
		
		if (fDistance < fPreviousDistance)
		{
			vCameraPosition = vPosition;
			fPreviousDistance = fDistance;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	
	return vCameraPosition;
}

void CCamera_Dungeon::Dungeon_Camera(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_vCurrentCameraPosition = Compute_DungeonCamera();

	/* Dungeon Camera*/
	if (!pGameInstance->Key_Pressing(VK_SHIFT))
	{
		_vector vTargetCameraPosition = XMVectorSetW(XMLoadFloat3(&m_vCurrentCameraPosition), 1.f) + XMVectorSet(0.f, 3.5f, -2.f, 1.f);
		_float3 vStoredTarget;
		XMStoreFloat3(&vStoredTarget, vTargetCameraPosition);
		m_pTransform->Go_TargetPosition(fTimeDelta, vStoredTarget, 0.f); 
	}
	else /* Debug Camera*/
	{
		// Camera Inputs
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
	_vector vCamPosition = m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vCamDirection = XMLoadFloat3(&vZoomPosition) - vCamPosition;

	vCamDirection -= XMVectorSet(0.f, .5f, .5f, 0.f);

	_float3 vDistance = _float3(0, 2.5, -2);

	_float3 vStoredCameraDirection;
	XMStoreFloat3(&vStoredCameraDirection, vCamDirection);

	if (fabsf(vStoredCameraDirection.y) < vDistance.y)
		return;

	vCamDirection = XMVector3Normalize(vCamDirection);
	vCamPosition += vCamDirection * 0.1f;

	m_pTransform->LookAt(XMVectorSetW(XMLoadFloat3(&vZoomPosition), 1.f));
	m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vCamPosition);
}

void CCamera_Dungeon::ZoomOut_Camera(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));

	_vector vPlayerPosition = pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_vector vCameraPosition = m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION);
	_float3 fDistance = _float3(0.f, 3.5f, -2.5);

	_vector vDirection = vPlayerPosition + XMLoadFloat3(&fDistance) - vCameraPosition;

	_float3 vStoredPlayerPosition;
	XMStoreFloat3(&vStoredPlayerPosition, vPlayerPosition);
	_float3 vStoredCameraPosition;
	XMStoreFloat3(&vStoredCameraPosition, vCameraPosition);

	if (fabsf(vStoredPlayerPosition.y + fDistance.y - vStoredCameraPosition.y) < 0.3f)
	{
		m_eCamMode = MODE_DUNGEON;

		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	vDirection = XMVector3Normalize(vDirection);
	vCameraPosition += vDirection * 0.1f;

	m_pTransform->LookAt(vPlayerPosition);
	m_pTransform->Set_State(CTransform::STATE::STATE_TRANSLATION, vCameraPosition);

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dungeon::Shaking_Camera(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));

	_float3 vPlayerPosition;
	XMStoreFloat3(&vPlayerPosition, pPlayer->Get_Transform()->Get_State(CTransform::STATE::STATE_TRANSLATION));

	m_iShakeCount++;

	if (m_iShakeCount % 4 == 0)
	{
		vPlayerPosition.y += m_fPower * m_fVelocity;
		if (rand() % 2 == 0)
			vPlayerPosition.z -= m_fPower * m_fVelocity;
		else
			vPlayerPosition.z += m_fPower * m_fVelocity;

		if (rand() % 2 == 0)
			vPlayerPosition.x -= m_fPower * m_fVelocity;
		else
			vPlayerPosition.x += m_fPower * m_fVelocity;
	}
	else if (m_iShakeCount % 4 == 1)
	{
		vPlayerPosition.y -= m_fPower * m_fVelocity;
		if (rand() % 2 == 0)
			vPlayerPosition.z -= m_fPower * m_fVelocity;
		else
			vPlayerPosition.z += m_fPower * m_fVelocity;

		if (rand() % 2 == 0)
			vPlayerPosition.x -= m_fPower * m_fVelocity;
		else
			vPlayerPosition.x += m_fPower * m_fVelocity;
	}

	m_fVelocity -= m_fVelocityDecrement;
	if (m_fVelocity < 0.0f)
	{
		m_eCamMode = MODE_DUNGEON;

		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_pTransform->LookAt(XMVectorSetW(XMLoadFloat3(&vPlayerPosition), 1.f));
	m_pTransform->Attach_ToTarget(XMVectorSetW(XMLoadFloat3(&vPlayerPosition), 1.f), XMVectorSet(0.f, 3.5f, -2.5, 1.f)); /* Distance from Camera to Player */

	RELEASE_INSTANCE(CGameInstance);
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