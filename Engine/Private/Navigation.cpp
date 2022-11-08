//#include "Navigation.h"
//#include "Cell.h"
//#include "Shader.h"
//#include "PipeLine.h"
//
//CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
//	: CComponent(pDevice, pContext)
//{
//}
//
//CNavigation::CNavigation(const CNavigation & rhs)
//	: CComponent(rhs)
//	, m_Cells(rhs.m_Cells)
//#ifdef _DEBUG
//	, m_pShader(rhs.m_pShader)
//#endif // _DEBUG
//{
//	for (auto& pCell : m_Cells)
//		Safe_AddRef(pCell);
//
//#ifdef _DEBUG
//	Safe_AddRef(m_pShader);
//#endif // _DEBUG
//}
//
//HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationData)
//{
//	HANDLE hFile = CreateFile(pNavigationData, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//	if (!hFile)
//		return E_FAIL;
//
//	_float3	vPoints[3];
//	_ulong dwByte = 0;
//
//	while (true)
//	{
//		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//		if (!dwByte)
//			break;
//
//		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
//		if (!pCell)
//			return E_FAIL;
//
//		m_Cells.push_back(pCell);
//	}
//
//	CloseHandle(hFile);
//
//	if (FAILED(SetUp_Neighbor()))
//		return E_FAIL;
//
//#ifdef _DEBUG
//	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
//	if (!m_pShader)
//		return E_FAIL;
//#endif // _DEBUG
//	return S_OK;
//}
//
//HRESULT CNavigation::Initialize(void * pArg)
//{
//	if (pArg)
//		memcpy(&m_NavDesc, pArg, sizeof(NAVDESC));
//
//	return S_OK;
//}
//
//_bool CNavigation::CanMove(_fvector vPosition)
//{
//	_int iNeighborIndex = -1;
//
//	/* Check if vPosition is inside the current NavigationCell.
//	That is, Object moved inside the NavigationCell it was currently in. */
//	if (m_Cells[m_NavDesc.iCurrentCellIndex]->Contain_Point(vPosition, &iNeighborIndex))
//		return true;
//	/* Object moved outside the NavigationCell it was currently in. */
//	else
//	{
//		/* Object moved to a Position which is contained inside an other NavigationCell. */
//		if (iNeighborIndex >= 0)
//		{
//			while (true)
//			{
//				if (iNeighborIndex  == -1)
//					return false;
//
//				if (m_Cells[iNeighborIndex]->Contain_Point(vPosition, &iNeighborIndex))
//					break;
//			}
//
//			m_NavDesc.iCurrentCellIndex = iNeighborIndex;
//			return true;
//		}
//		/* Object moved to a Position which is not inside any NavigationCell. */
//		else
//		{
//			/* TODO: Add return logic for Sliding here. */
//			return false;
//		}
//	}
//}
//
//#ifdef _DEBUG
//HRESULT CNavigation::Render_Navigation()
//{
//	_float4x4 WorldMatrix;
//	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
//
//	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
//	m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
//	m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
//	RELEASE_INSTANCE(CPipeLine);
//
//	if (m_NavDesc.iCurrentCellIndex  == -1)
//	{
//		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
//		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
//		m_pShader->Begin(0);
//
//		for (auto& pCell : m_Cells)
//		{
//			if (pCell)
//				pCell->Render();
//		}
//	}
//	else
//	{
//		WorldMatrix._24 = 0.1f; // Set Y Translation.
//		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
//		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));
//
//		m_pShader->Begin(0);
//		m_Cells[m_NavDesc.iCurrentCellIndex]->Render();
//	}
//
//	return S_OK;
//}
//#endif // _DEBUG
//
//HRESULT CNavigation::SetUp_Neighbor()
//{
//	for (auto& pSourCell : m_Cells)
//	{
//		for (auto& pDestCell : m_Cells)
//		{
//			if (pSourCell == pDestCell)
//				continue;
//
//			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
//				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
//			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
//				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
//			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
//				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
//		}
//	}
//
//	return S_OK;
//}
//
//CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationData)
//{
//	CNavigation* pInstance = new CNavigation(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype(pNavigationData)))
//	{
//		ERR_MSG(TEXT("Failed to Create: CNavigation"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CComponent * CNavigation::Clone(void * pArg)
//{
//	CNavigation* pInstance = new CNavigation(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		ERR_MSG(TEXT("Failed to Clone: CNavigation"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void CNavigation::Free()
//{
//	__super::Free();
//
//	for (auto& pCell : m_Cells)
//		Safe_Release(pCell);
//
//	m_Cells.clear();
//
//#ifdef _DEBUG
//	Safe_Release(m_pShader);
//#endif // _DEBUG
//}