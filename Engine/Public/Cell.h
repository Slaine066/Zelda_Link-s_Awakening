//#pragma once
//
//#include "Base.h"
//
//BEGIN(Engine)
//class CCell final : public CBase
//{
//public:
//	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
//	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
//
//private:
//	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual ~CCell() = default;
//
//public:
//	const _float3* Get_Point(POINT ePoint) { return &m_vPoints[ePoint]; }
//	void Set_Neighbor(LINE eLine, const CCell* pCell) { m_iNeighborIndices[eLine] = pCell->m_iIndex; }
//
//public:
//	HRESULT Initialize(const _float3* pPoints, _int iIndex);
//	_bool Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint);
//	_bool Contain_Point(_fvector vPosition, _int* pNeighborIndex);
//
//#ifdef _DEBUG
//	HRESULT Render();
//#endif // _DEBUG
//
//private:
//	ID3D11Device* m_pDevice = nullptr;
//	ID3D11DeviceContext* m_pContext = nullptr;
//
//private:
//	_int m_iIndex = 0;
//	_float3	m_vPoints[POINT_END];
//	_float3	m_vNormals[LINE_END];
//
//	_int m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
//
//#ifdef _DEBUG
//	class CVIBuffer_Navigation*	m_pVIBuffer = nullptr;
//#endif // _DEBUG
//
//public:
//	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
//	virtual void Free() override;
//};
//END