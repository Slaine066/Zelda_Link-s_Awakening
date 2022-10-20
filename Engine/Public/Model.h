#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice ,ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);


public:
	_uint	Get_NumMeshContainers() const { return m_iNumMeshes; }

public:
	HRESULT SetUp_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);

private:
	const aiScene*				m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

private:
	TYPE								m_eModelType = TYPE_END;

	_uint								m_iNumMeshes = 0;
	vector<class CMeshContainer*>		m_Meshes;

private:
	_uint								m_iNumMaterials = 0;
	vector<MODELMATERIAL>				m_Materials;

private:
	_float4x4 m_PivotMatrix;

private:
	HRESULT Create_MeshContainer();
	HRESULT Create_Materials(const char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END