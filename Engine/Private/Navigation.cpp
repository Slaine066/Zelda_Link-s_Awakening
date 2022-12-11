#include "Navigation.h"
#include "Shader.h"
#include "PipeLine.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG
}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationData)
{
	HANDLE hFile = CreateFile(pNavigationData, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hFile)
		return E_FAIL;

	pair<CCell::CELL_TYPE, _float3x3> vPoints;
	_ulong dwByte = 0;

	while (true)
	{
		ReadFile(hFile, &vPoints, sizeof(pair<CCell::CELL_TYPE, _float3x3>), &dwByte, nullptr);
		if (!dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
		if (!pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Setup_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (!m_pShader)
		return E_FAIL;
#endif // _DEBUG
	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (pArg)
		memcpy(&m_NavDesc, pArg, sizeof(NAVDESC));

	Compute_CurrentCell();

	return S_OK;
}

_float CNavigation::Get_NavigationHeight(_float3 vPosition)
{
	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&vPosition);
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	_vector vPointA = XMLoadFloat3(m_Cells[m_NavDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_A));
	vPointA = XMVectorSetW(vPointA, 1.f);
	_vector vPointB = XMLoadFloat3(m_Cells[m_NavDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_B));
	vPointB = XMVectorSetW(vPointB, 1.f);
	_vector vPointC = XMLoadFloat3(m_Cells[m_NavDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_C));
	vPointC = XMVectorSetW(vPointC, 1.f);

	if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
	{
		_float3 vIntersection;
		XMStoreFloat3(&vIntersection, vRayPos + vRayDir * fDistance);

		return vIntersection.y;
	}

	return 0.f;
}

_bool CNavigation::Get_PointOnNavigation(OUT _float3 vPosition)
{
	if (m_Cells[m_NavDesc.iCurrentCellIndex]->Get_CellType() == CCell::CELL_TYPE::CELL_NOBLOCK)
		return false;

	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&vPosition);
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	_vector vPointA = XMLoadFloat3(m_Cells[m_NavDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_A));
	vPointA = XMVectorSetW(vPointA, 1.f);
	_vector vPointB = XMLoadFloat3(m_Cells[m_NavDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_B));
	vPointB = XMVectorSetW(vPointB, 1.f);
	_vector vPointC = XMLoadFloat3(m_Cells[m_NavDesc.iCurrentCellIndex]->Get_Point(CCell::POINT::POINT_C));
	vPointC = XMVectorSetW(vPointC, 1.f);

	if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
	{
		XMStoreFloat3(&vPosition, vRayPos + vRayDir * fDistance);
		return true;
	}

	return false;
}

CCell::CELL_TYPE CNavigation::Get_CellType()
{
	return m_Cells[m_NavDesc.iCurrentCellIndex]->Get_CellType();
}

_bool CNavigation::CanMove(_fvector vPosition)
{
	_int iNeighborIndex = -1;

	/* Check if vPosition is inside the current NavigationCell.
	That is, Object moved inside the NavigationCell it was currently in. */
	if (m_Cells[m_NavDesc.iCurrentCellIndex]->Contain_Point(vPosition, &iNeighborIndex))
		return true;
	/* Object moved outside the NavigationCell it was currently in. */
	else
	{
		/* Object moved to a Position which is contained inside an other NavigationCell. */
		if (iNeighborIndex >= 0)
		{
			while (true)
			{
				if (iNeighborIndex  == -1)
					return false;

				if (m_Cells[iNeighborIndex]->Contain_Point(vPosition, &iNeighborIndex))
					break;
			}

			m_NavDesc.iCurrentCellIndex = iNeighborIndex;
			return true;
		}
		/* Object moved to a Position which is not inside any NavigationCell. */
		else
		{
			/* TODO: Add return logic for Sliding here. */
			return false;
		}
	}
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	m_pShader->Set_RawValue("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShader->Set_RawValue("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	RELEASE_INSTANCE(CPipeLine);

	_float4 vGreen = _float4(.31f, .78f, .47f, 1.f);	/* Block Cell. */
	_float4 vOrange = _float4(1.f, .5f, .31f, 1.f);		/* No Block Cell. */
	_float4 vRed = _float4(.88f, .19f, .38f, 1.f);		/* Current Cell. */
	

	if (m_NavDesc.iCurrentCellIndex  == -1)
	{
		WorldMatrix._24 = .02f;
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &vGreen, sizeof(_float4));
		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
		{
			if (pCell && pCell->Get_CellType() == CCell::CELL_TYPE::CELL_BLOCK)
				pCell->Render();
		}

		m_pShader->Set_RawValue("g_vColor", &vOrange, sizeof(_float4));
		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
		{
			if (pCell && pCell->Get_CellType() == CCell::CELL_TYPE::CELL_NOBLOCK)
				pCell->Render();
		}
	}
	else
	{
		WorldMatrix._24 = .04f; // Set Y Translation.
		m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShader->Set_RawValue("g_vColor", &vRed, sizeof(_float4));

		m_pShader->Begin(0);
		m_Cells[m_NavDesc.iCurrentCellIndex]->Render();
	}

	return S_OK;
}
#endif // _DEBUG

void CNavigation::Compute_CurrentCell()
{
	_float fDistance = 0.f;

	_vector vRayPos = XMLoadFloat3(&m_NavDesc.vInitialPosition);
	vRayPos += XMVectorSet(0.f, 10.f, 0.f, 1.f);
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	for (_uint i = 0; i < m_Cells.size(); i++)
	{
		_vector vPointA = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_A));
		vPointA = XMVectorSetW(vPointA, 1.f);
		_vector vPointB = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_B));
		vPointB = XMVectorSetW(vPointB, 1.f);
		_vector vPointC = XMLoadFloat3(m_Cells[i]->Get_Point(CCell::POINT::POINT_C));
		vPointC = XMVectorSetW(vPointC, 1.f);

		if (TriangleTests::Intersects(vRayPos, vRayDir, vPointA, vPointB, vPointC, fDistance))
		{
			m_NavDesc.iCurrentCellIndex = i;
			return;
		}
	}
}

HRESULT CNavigation::Setup_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
			if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationData)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationData)))
	{
		ERR_MSG(TEXT("Failed to Create: CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CNavigation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG
}