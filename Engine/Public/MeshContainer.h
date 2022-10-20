#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

/* 모델을 구성하는 각각의 메시다. */
class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }

private:
	const aiMesh* m_pAIMesh = nullptr;
	_uint m_iMaterialIndex = 0;

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END