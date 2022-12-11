#pragma once

#include "Component.h"
#include "Cell.h"

BEGIN(Engine)
class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int iCurrentCellIndex = -1;
		_float3 vInitialPosition;
	}NAVDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationData);
	virtual HRESULT Initialize(void* pArg);
	 
public:
	void Compute_CurrentCell();
	void Set_InitialPosition(_float3 vPosition) { m_NavDesc.vInitialPosition = vPosition; }
	_float Get_NavigationHeight(_float3 vPosition);
	_bool Get_PointOnNavigation(OUT _float3 vPosition);
	CCell::CELL_TYPE Get_CellType();
	_bool CanMove(_fvector vPosition);

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	NAVDESC m_NavDesc;
	vector<CCell*> m_Cells;

#ifdef _DEBUG
	class CShader* m_pShader = nullptr;
#endif // _DEBUG

private:
	HRESULT Setup_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationData);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END