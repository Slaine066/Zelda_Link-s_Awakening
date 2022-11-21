#include "Transform.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	WorldMatrix.r[eState] = vState;
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CTransform::Set_Scale(STATE eState, _float fScale)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	WorldMatrix.r[eState] = XMVector3Normalize(WorldMatrix.r[eState]) * fScale;
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (pArg != nullptr)
	{
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

		// Set Initial WorldMatrix
		m_WorldMatrix = m_TransformDesc.vInitialWorldMatrix;
	}

	return S_OK;
}

void CTransform::Move_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (!pNavigation)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else if (pNavigation->CanMove(vPosition))
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Move_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector4Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (!pNavigation)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else if (pNavigation->CanMove(vPosition))
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Move_Left(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (!pNavigation)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else if (pNavigation->CanMove(vPosition))
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Move_Right(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	if (!pNavigation)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else if (pNavigation->CanMove(vPosition))
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Move_Direction(_vector vDirection, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	vPosition += vDirection * m_TransformDesc.fSpeedPerSec * fTimeDelta;
	
	if (!pNavigation)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else if (pNavigation->CanMove(vPosition))
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

/* This is NOT an additive rotation like the "Turn()" function above.
This function set the specific rotation on the specific axis. */
void CTransform::Set_Rotation(_float3 fAngle)
{
	// Get current RotationMatrix from the WorldMatrix by decomposition.
	_vector vScale, vRotationQuat, vTranslation;
	XMMatrixDecompose(&vScale, &vRotationQuat, &vTranslation, Get_WorldMatrix());
	_matrix RotationMatrix = XMMatrixRotationQuaternion(vRotationQuat);

	// Multiply the WorldMatrix by the Inverse of current RotationMatrix (to get a WorldMatrix without any Rotation)
	_matrix InverseRotationMatrix = XMMatrixInverse(nullptr, RotationMatrix);
	_matrix WorldMatrixWithoutRotation = XMMatrixMultiply(Get_WorldMatrix(), InverseRotationMatrix);

	// Make a NewRotationMatrix with new angle values
	_matrix NewRotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fAngle.x), XMConvertToRadians(fAngle.y), XMConvertToRadians(fAngle.z));
	m_fCurrentRotationX = fAngle.x;
	m_fCurrentRotationY = fAngle.y;
	m_fCurrentRotationZ = fAngle.z;

	// Set NewRotationMatrix to WorldMatrixWithoutRotation
	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[0], NewRotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[1], NewRotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(WorldMatrixWithoutRotation.r[2], NewRotationMatrix));
}

void CTransform::Set_RotationY(_float fAngleY)
{
	Set_Rotation(_float3(m_fCurrentRotationX, fAngleY, m_fCurrentRotationZ));
}

void CTransform::LookAt(_fvector vAt)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);

	_vector	vLook = vAt - vPosition;
	_vector	vAxisY = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector	vRight = XMVector3Cross(vAxisY, vLook);
	_vector	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * Get_Scale(CTransform::STATE_RIGHT));
	Set_State(STATE_UP, XMVector3Normalize(vUp) * Get_Scale(CTransform::STATE_UP));
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * Get_Scale(CTransform::STATE_LOOK));
}

void CTransform::Attach_ToTarget(_fvector fTargetPosition, _fvector fDistance)
{
	_float4 vTargetPos;
	XMStoreFloat4(&vTargetPos, fTargetPosition);

	_float4 vDistance;
	XMStoreFloat4(&vDistance, fDistance);

	_float4 vTranslation = { vTargetPos.x + vDistance.x, vTargetPos.y + vDistance.y, vTargetPos.z + vDistance.z, 1.f };
	Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vTranslation));
}

_bool CTransform::Go_TargetPosition(_float fTimeDelta, _float3 vTargetPosition, _float fDistance, CNavigation* pNavigation)
{
	_vector vPos = Get_State(CTransform::STATE::STATE_TRANSLATION);
	_float3 vTargetPos = { vTargetPosition.x, vTargetPosition.y, vTargetPosition.z };

	_vector vDirection = XMVectorSet(vTargetPos.x, vTargetPos.y, vTargetPos.z, 1.f) - vPos;
	vDirection = XMVector4Normalize(vDirection);

	vPos += vDirection * fTimeDelta * m_TransformDesc.fSpeedPerSec;

	_float fDist = XMVectorGetX(XMVector3Length(vPos - XMLoadFloat3(&vTargetPos)));
	if (fDist < fDistance + 0.1f) /* Need to add 0.1f in case "fDistance" is 0. */
		return true;

	if (!pNavigation)
		Set_State(CTransform::STATE_TRANSLATION, vPos);
	else if (pNavigation->CanMove(vPos))
		Set_State(CTransform::STATE_TRANSLATION, vPos);

	return false;
}

CTransform* CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}