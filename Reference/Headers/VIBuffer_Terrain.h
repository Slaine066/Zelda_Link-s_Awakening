#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);

private:
	_uint m_iNumVerticesX = 0, m_iNumVerticesZ = 0;

public:
	static CVIBuffer_Terrain* CreateWithVertices(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX = 0, _uint iNumVerticesZ = 0);
	static CVIBuffer_Terrain* CreateWithHeightMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath = nullptr);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
