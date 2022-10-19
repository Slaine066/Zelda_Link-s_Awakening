#include "..\Public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


HRESULT CGameObject::Add_Components(const _tchar * pComponentTag, _uint iLevelIndex, const _tchar * pPrototypeTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Compute_CamDistance(_float3 vWorldPos)
{
	_float4x4			ViewMatrix;

	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	//D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);

	//_float3		vCamPosition = *(_float3*)&ViewMatrix.m[3][0];

	//m_fCamDistance = D3DXVec3Length(&(vCamPosition - vWorldPos));
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)	
		Safe_Release(Pair.second);
	m_Components.clear();	

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
