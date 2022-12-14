#pragma once

/* Level Manager Class
- Stores a Level* of the current Level (Level Index is also stored);
- Update Level;
- Clean up Resources of the previous Level when changing Level. */

#include "Base.h"

BEGIN(Engine)
class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
public:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	_uint Get_CurrentLevelIndex() { return m_iLevelIndex; }
	_uint Get_NextLevelIndex() { return m_iNextLevelIndex; }
	_bool Get_IsJustSpawned() { return m_bIsJustSpawned; }
	list<class CTriggerBox*> Get_TriggerBoxes();
	class CTriggerBox* Get_TriggerBox(char* pTriggerBoxName);
	char* Get_SpawnTriggerBoxName() { return m_pSpawnTriggerBoxName; }
	class CComponent* Get_CurrentNavigationMesh();

	void Set_NextLevelIndex(_uint iNextLevelIndex) { m_iNextLevelIndex = iNextLevelIndex; }
	void Set_IsJustSpawned(_bool bIsJustSpawned) { m_bIsJustSpawned = bIsJustSpawned; }
	void Set_SpawnTriggerBox(char* pSpawnTriggerBox);

public:
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

private:
	class CLevel* m_pCurrentLevel = nullptr;
	_uint m_iLevelIndex;
	_uint m_iNextLevelIndex;

	char m_pSpawnTriggerBoxName[MAX_PATH] = "";
	_bool m_bIsJustSpawned = false;

public:
	virtual void Free() override;
};
END