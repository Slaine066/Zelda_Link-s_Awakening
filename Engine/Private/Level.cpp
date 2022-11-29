#include "Level.h"
#include "GameInstance.h"
#include "TriggerBox.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice); 
	Safe_AddRef(m_pContext);
}

CTriggerBox * CLevel::Get_TriggerBox(char * pTriggerBoxName)
{
	for (auto& pTrigger : m_TriggerBoxes)
	{
		if (!strcmp(pTrigger->Get_TriggerBoxDesc().pTriggerName, pTriggerBoxName))
			return pTrigger;
	}

	return nullptr;
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
	for (auto& pTriggerBox : m_TriggerBoxes)
		pTriggerBox->Tick(fTimeDelta);
}

void CLevel::Late_Tick(_float fTimeDelta)
{
	for (auto& pTriggerBox : m_TriggerBoxes)
		if (pTriggerBox->Late_Tick(fTimeDelta))
			break;
}

void CLevel::Render_NavigationMesh()
{
	if (!m_pNavigationMesh)
		return;

	((CNavigation*)m_pNavigationMesh)->Render_Navigation();
}

void CLevel::Render_TriggerBox()
{
	if (m_TriggerBoxes.empty())
		return;

	for (auto& pTriggerBox : m_TriggerBoxes)
		pTriggerBox->Render();
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

	for (auto& pTrigger : m_TriggerBoxes)
		Safe_Release(pTrigger);

	m_TriggerBoxes.clear();
}