#include "..\Public\Camera.h"

#include "PipeLine.h"

_tchar*	CCamera::m_pTransformTag = TEXT("Com_Transform");

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::Get_Instance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;	

	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	m_Components.emplace(m_pTransformTag, m_pTransform);

	Safe_AddRef(m_pTransform);

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_OnPipeLine()
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	/* 카메라의 트랜스폼으로 부터 월드행렬을 가져와서 역행렬을 구한다음(뷰행렬을 구했다.) 파이프라인에 저장한다. */
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, XMMatrixInverse(nullptr, m_pTransform->Get_WorldMatrix()));
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTransform);

}
