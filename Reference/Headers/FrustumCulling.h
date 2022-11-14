#pragma once

#include "Base.h"

BEGIN(Engine)
class CFrustumCulling final : public CBase
{
	DECLARE_SINGLETON(CFrustumCulling)
private:
	CFrustumCulling();
	virtual ~CFrustumCulling() = default;

public:
	HRESULT Initialize();				/* Define the 8 Points defining the Frustum in Projection Space. */
	void Transform_ToWorldSpace();	/* Convert the 8 Point to World Space. */

	_bool IsIn_Frustum(_fvector vPosition, _float fRange); /* World Space comparing Function! */

private:
	_float3	m_vOriginalPoints[8];
	_float3	m_vWorldPoints[8];

	_float4	m_WorldPlane[6];

private:
	void BuildPlanes(_In_ const _float3* pPoints, _Out_ _float4* pPlanes);

public:
	virtual void Free() override;
};
END