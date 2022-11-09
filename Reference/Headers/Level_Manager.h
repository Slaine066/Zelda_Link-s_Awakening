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

public:
	HRESULT Open_Level(unsigned int iLevelIndex, class CLevel* pNewLevel);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

	void Render_NavigationMesh();

private:
	class CLevel* m_pCurrentLevel = nullptr;
	_uint m_iLevelIndex;

public:
	virtual void Free() override;
};
END