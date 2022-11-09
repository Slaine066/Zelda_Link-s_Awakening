#pragma once

#include "Base.h"

/* Parent Class of the Levels defined in the Client. */

BEGIN(Engine)
class CComponent;
class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	CComponent* Get_NavigationMesh() { return m_pNavigationMesh; }

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

	void Render_NavigationMesh();

protected:
	ID3D11Device* m_pDevice = nullptr; 
	ID3D11DeviceContext* m_pContext = nullptr;

	/* Navigation Mesh is stored inside the Level Class. 
	Can't store it in the Terrain Class cause Meshes are used instead.
	However can't even store in the Mesh Class cause the Map is composed by many Meshes. */
	CComponent* m_pNavigationMesh = nullptr;

	HRESULT Add_NavigationMesh(const _tchar* pNavigationMeshTag, _uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent** ppOut, void* pArg = nullptr);

public:
	virtual void Free() override;
};
END