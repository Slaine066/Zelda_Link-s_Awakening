#include "FrustumCulling.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustumCulling)

CFrustumCulling::CFrustumCulling()
{
}

HRESULT CFrustumCulling::Initialize()
{
	m_vOriginalPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vOriginalPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vOriginalPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vOriginalPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vOriginalPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vOriginalPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vOriginalPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vOriginalPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustumCulling::Transform_ToWorldSpace()
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix	ProjMatrixInv = XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	_matrix	ViewMatrixInv = XMMatrixInverse(nullptr, pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	RELEASE_INSTANCE(CPipeLine);

	_vector	vPoints[8];

	for (_uint i = 0; i < 8; ++i)
	{
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), ProjMatrixInv);
		vPoints[i] = XMVector3TransformCoord(vPoints[i], ViewMatrixInv);

		XMStoreFloat3(&m_vWorldPoints[i], vPoints[i]);
	}

	BuildPlanes(m_vWorldPoints, m_WorldPlane);
}

_bool CFrustumCulling::IsIn_Frustum(_fvector vPosition, _float fRange)
{
	/* a, b, c, d */
	/* x, y, z, 1 */
	/* ax + by + cz + d = ? */

	/* ? == 0	-> Point belongs to Plane. */
	/* ? > 0	-> Point is outside the Plane. */
	/* ? < 0	-> Point is inside the Plane. */

	/* ?		-> Distance between Plane and Point. */

	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlane[i]), vPosition)))
			return false;
	}

	return true;
}

void CFrustumCulling::BuildPlanes(const _float3 * pPoints, _float4 * pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6]))); /* +X */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3]))); /* -X */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1]))); /* +Y */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6]))); /* -Y */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7]))); /* +Z */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2]))); /* -Z */
}

void CFrustumCulling::Free()
{
}