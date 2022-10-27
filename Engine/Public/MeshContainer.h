#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

/* 모델을 구성하는 각각의 메시다. */
class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	void Get_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT SetUp_Bones(class CModel* pModel);

private:
	char			m_szName[MAX_PATH] = "";
	const aiMesh*	m_pAIMesh = nullptr;
	_uint			m_iMaterialIndex = 0;

	/* 이 메시에 영향을 주는 뼈들의 갯수 .*/
	_uint							m_iNumBones = 0;
	vector<class CHierarchyNode*>	m_Bones;

public:
	HRESULT Create_VertexBuffer_NonAnimModel(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Create_VertexBuffer_AnimModel(const aiMesh* pAIMesh, class CModel* pModel);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END