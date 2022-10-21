#include "Picking.h"
#include "Transform.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinCX, _uint iWinCY)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_hWnd = hWnd;

	m_iWinCX = iWinCX;
	m_iWinCY = iWinCY;

	return S_OK;
}

void CPicking::Tick()
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_float3	vMousePos;

	// Get Mouse Position in Projection Space
	vMousePos.x = _float(ptMouse.x / (m_iWinCX * 0.5f) - 1);
	vMousePos.y = _float(ptMouse.y / (m_iWinCY * -0.5f) + 1);
	vMousePos.z = 0.f;

	_vector vMousePosition = XMLoadFloat3(&vMousePos);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	// Get Mouse Position in View Space
	_matrix ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	_matrix ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrix);
	vMousePosition = XMVector3TransformCoord(vMousePosition, ProjMatrixInv);

	// Get Ray Position and Ray Direction in View Space
	_vector vRayPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_vector vRayDirection = vMousePosition - vRayPosition;

	// Get Ray Position and Ray Direction in World Space
	_matrix ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrix);
	vRayPosition = XMVector3TransformCoord(vRayPosition, ViewMatrixInv);
	vRayDirection = XMVector3TransformNormal(vRayDirection, ViewMatrixInv);
	vRayDirection = XMVector3Normalize(vRayDirection);

	XMStoreFloat3(&m_vRayPosition, vRayPosition);
	XMStoreFloat3(&m_vRayDirection, vRayDirection);

	Safe_Release(pGameInstance);
}

void CPicking::Compute_LocalRayInfo(CTransform* pTransform, _vector& vRayPosition, _vector& vRayDirection)
{
	_matrix WorldMatrix = pTransform->Get_WorldMatrix();
	_matrix WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);

	vRayPosition = XMVector3TransformCoord(XMLoadFloat3(&m_vRayPosition), WorldMatrixInv);
	vRayDirection = XMVector3TransformNormal(XMLoadFloat3(&m_vRayDirection), WorldMatrixInv);
	vRayDirection = XMVector3Normalize(vRayDirection);
}

void CPicking::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}