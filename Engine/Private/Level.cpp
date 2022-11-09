#include "Level.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
}

void CLevel::Late_Tick(_float fTimeDelta)
{
}

void CLevel::Render_NavigationMesh()
{
	if (!m_pNavigationMesh)
		return;

	((CNavigation*)m_pNavigationMesh)->Render_Navigation();
}

HRESULT CLevel::Add_NavigationMesh(const _tchar* pNavigationMeshTag, _uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent** ppOut, void* pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (!pComponent)
		return E_FAIL;

	*ppOut = pComponent;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel::Free()
{
  	if (m_pNavigationMesh)
		Safe_Release(m_pNavigationMesh);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}