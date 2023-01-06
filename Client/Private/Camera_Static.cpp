#include "stdafx.h"

#include "Camera_Static.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Static::CCamera_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Static::CCamera_Static(const CCamera_Static & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Static::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Static::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera_Static::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	switch (m_eCamMode)
	{
	case MODE_STATIC:
		Static_Camera(fTimeDelta);
		break;
	case MODE_ZOOMIN:
		ZoomIn_Camera(fTimeDelta, m_vZoomPosition);
		break;
	case MODE_ZOOMOUT:
		ZoomOut_Camera(fTimeDelta);
		break;
	}

	if (FAILED(Bind_OnPipeLine()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

_uint CCamera_Static::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

HRESULT CCamera_Static::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Static::Static_Camera(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* Static Camera*/
	if (!pGameInstance->Key_Pressing(VK_SHIFT))
	{
		m_pTransform->LookAt(XMLoadFloat4(&m_vLookPosition));
		m_pTransform->Attach_ToTarget(XMLoadFloat4(&m_vLookPosition), XMVectorSet(0.f, 3.5f, -2.f, 1.f)); /* Distance from Camera to Player */
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

void CCamera_Static::ZoomIn_Camera(_float fTimeDelta, _float3 vZoomPosition)
{
	vZoomPosition.y += 2.f;
	vZoomPosition.z -= 1.f;

	m_pTransform->Go_TargetPosition(fTimeDelta / 5, vZoomPosition, 0.f);
}

void CCamera_Static::ZoomOut_Camera(_float fTimeDelta)
{
	if (m_pTransform->Go_TargetPosition(fTimeDelta / 5, _float3(m_vLookPosition.x, m_vLookPosition.y + 3.5f, m_vLookPosition.z - 2.f), 0.f))
	{
		XMStoreFloat4(&m_vLookPosition, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
		m_vLookPosition.y -= 3.5f;
		m_vLookPosition.z += 2.f;

		m_eCamMode = MODE_STATIC;
	}
}

CCamera_Static * CCamera_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Static* pInstance = new CCamera_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CCamera_Static"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Static::Clone(void* pArg)
{
	CCamera_Static* pInstance = new CCamera_Static(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CCamera_Static"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Static::Free()
{
	__super::Free();
}