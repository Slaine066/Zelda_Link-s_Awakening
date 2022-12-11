#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

list<CTriggerBox*> CLevel_Manager::Get_TriggerBoxes()
{
	if (m_pCurrentLevel == nullptr)
		return list<CTriggerBox*>();

	return m_pCurrentLevel->Get_TriggerBoxes();
}

CTriggerBox * CLevel_Manager::Get_TriggerBox(char * pTriggerBoxName)
{
	if (m_pCurrentLevel == nullptr)
		return nullptr;

	return m_pCurrentLevel->Get_TriggerBox(pTriggerBoxName);
}

CComponent * CLevel_Manager::Get_CurrentNavigationMesh()
{
	if (m_pCurrentLevel == nullptr)
		return nullptr;

	return m_pCurrentLevel->Get_NavigationMesh();
}

void CLevel_Manager::Set_SpawnTriggerBox(char * pSpawnTriggerBox)
{
	if (!pSpawnTriggerBox)
	{
		pSpawnTriggerBox = "";
		return;
	}
	else
		strcpy_s(m_pSpawnTriggerBoxName, MAX_PATH, pSpawnTriggerBox);
}

HRESULT CLevel_Manager::Open_Level(unsigned int iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Clear(m_iLevelIndex);

		Safe_Release(pGameInstance);
	}

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	m_iLevelIndex = iLevelIndex;

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (m_pCurrentLevel == nullptr)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

void CLevel_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_pCurrentLevel == nullptr)
		return;

	m_pCurrentLevel->Late_Tick(fTimeDelta);
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}