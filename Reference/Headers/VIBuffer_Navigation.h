#pragma once

#include "VIBuffer.h"

/* Vertex/Index Buffer used to store Vertices/Indices of ONE Cell of the Navigation Mesh. 
Every single Navigation Mesh Cell will hold this Buffer. */

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Navigation final : public CVIBuffer
{
protected:
	CVIBuffer_Navigation(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Navigation(const CVIBuffer_Navigation& rhs);
	virtual ~CVIBuffer_Navigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Render() override;

public:
	static CVIBuffer_Navigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
