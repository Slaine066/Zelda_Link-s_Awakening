#pragma once

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int iCurrentCellIndex = -1;
	}NAVDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationData);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool CanMove(_fvector vPosition);

#ifdef _DEBUG
	HRESULT Render_Navigation();
#endif // _DEBUG

private:
	NAVDESC m_NavDesc;
	vector<class CCell*> m_Cells;

#ifdef _DEBUG
	class CShader* m_pShader = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbor();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationData);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END