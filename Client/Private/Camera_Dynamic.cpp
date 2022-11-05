#include "stdafx.h"

#include "Camera_Dynamic.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float4 vObjPosition;
	XMStoreFloat4(&vObjPosition, m_pTransform->Get_State(CTransform::STATE::STATE_TRANSLATION));
	Set_Position(_float3(vObjPosition.x, vObjPosition.y, vObjPosition.y));

	// Camera Inputs
	if (GetKeyState('W') < 0)
		m_pTransform->Move_Straight(fTimeDelta);
	if (GetKeyState('S') < 0)
		m_pTransform->Move_Backward(fTimeDelta);
	if (GetKeyState('A') < 0)
		m_pTransform->Move_Left(fTimeDelta);
	if (GetKeyState('D') < 0)
		m_pTransform->Move_Right(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	_long MouseMove = 0;
	if ((MouseMove = pGameInstance->Get_DIMMoveState(DIMM_X)) && GetKeyState(VK_SHIFT) < 0)
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.1f);

	if ((MouseMove = pGameInstance->Get_DIMMoveState(DIMM_Y)) && GetKeyState(VK_SHIFT) < 0)
		m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.1f);

	Safe_Release(pGameInstance);

	

	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//CGameObject* pGameObject = pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player")); // TODO: Level should be the current level
	//if (!pGameObject)
	//	return OBJ_NOEVENT;

	//_float3 vPos = pGameObject->Get_Position();
	//_vector vPosition = XMLoadFloat3(&_float3(vPos.x, vPos.y, vPos.z));
	//XMVectorSetW(vPosition, 1.f);

	//m_pTransform->LookAt(vPosition);
	//m_pTransform->Follow_Target(vPosition, XMVectorSet(0.f, 6.f, -10, 1.f));

	if (FAILED(Bind_OnPipeLine()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	
	return S_OK;
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic* pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();
}