#include "Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "Shader.h"
#include "HierarchyNode.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_Materials(rhs.m_Materials)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_iNumBones(rhs.m_iNumBones)
	, m_eModelType(rhs.m_eModelType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pMeshContainer : rhs.m_Meshes)
		m_Meshes.push_back((CMeshContainer*)pMeshContainer->Clone());

	for (auto& Material : m_Materials)
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pMatTextures[i]);
}

char * CModel::Get_MeshName(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MeshName();
}

char * CModel::Get_AnimationName(_uint iAnimationIndex)
{
	return m_Animations[iAnimationIndex]->Get_AnimationName();
}

char * CModel::Get_CurrentAnimationName()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimationName();
}

CHierarchyNode* CModel::Get_BonePtr(const char * pBoneName) const
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pBoneName);
	});

	return *iter;
}

void CModel::Reset_CurrentAnimation()
{
	m_Animations[m_iCurrentAnimIndex]->Reset_Animation();
}

HRESULT CModel::Initialize_Prototype(TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix)
{
	m_eModelType = eModelType;
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	_uint iFlag = 0;

	if(TYPE_NONANIM == eModelType)	
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	
	// Imported reads the *.fbx File and populate the Assimp structure (aiScene)
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	if (FAILED(Create_MeshContainer()))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;

	// Bones are not created when the Model Prototype is created, but instead when is cloned.
	// By "Deep Copy"ing while cloning we avoid aving same bones pointers in different models.

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	if (m_eModelType == TYPE::TYPE_NONANIM)
		return S_OK;

	/* Load all the Model Bones (aiNode) */
	if (FAILED(Create_HierarchyNodes(m_pAIScene->mRootNode)))
		return E_FAIL;

	/* Load all the Mesh Bones 
	Why can't I just use the Model Bones? Because while rendering we just need the Bones which affect the Vertices.
	*/
	for (auto& pMeshContainer : m_Meshes)
		pMeshContainer->SetUp_Bones(this);

	if (FAILED(Create_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::SetUp_Material(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();
	CTexture* pMaterialTexture = m_Materials[iMaterialIndex].pMatTextures[eType];

	if (!pMaterialTexture)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRV = pMaterialTexture->Get_SRV();

	return pShader->Set_ShaderResourceView(pConstantName, pSRV);
}

HRESULT CModel::Play_Animation(_float fTimeDelta, OUT _bool& bIsFinished, _bool bIsLoop)
{
	/* Update Bone m_TransformationMatrix. */
	bIsFinished = m_Animations[m_iCurrentAnimIndex]->Animate(fTimeDelta, bIsLoop);

	/* Update Bone m_CombinedTransformationMatrix. */
	for (auto& pBoneNode : m_Bones)
		pBoneNode->Invalidate_CombinedTransformationmatrix();

	return S_OK;
}

/* 
This function will be called N times by the Model owner. 
Where N is the number of Meshes in this Model.
*/
HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex, _bool bAnimate)
{
	if (m_eModelType == TYPE::TYPE_ANIM && bAnimate)
	{
		/* Get the matrix of affected bones per mesh. */
		_float4x4 BoneMatrix[256];

		/* Populate Bone Matrix Array with the Bone Matrices of every Mesh. */
		m_Meshes[iMeshIndex]->Get_BoneMatrices(BoneMatrix, XMLoadFloat4x4(&m_PivotMatrix));

		// Give it to the Shader
		pShader->Set_MatrixArray("g_BoneMatrices", BoneMatrix, 256);
	}
	
	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

// Mesh Creation
/*
For every "aiMesh" stored in the "aiScene" get the followings:
Vertex Positions, Normals, Texture UVs, Tangents, Bone Indexes, Bone Influences.
Create VertexBuffer and IndexBuffer with above data.
*/
HRESULT CModel::Create_MeshContainer()
{
	if (m_pAIScene == nullptr)
		return E_FAIL;

	// One Model consists of multiple Meshes
	m_iNumMeshes = m_pAIScene->mNumMeshes;
	m_Meshes.reserve(m_iNumMeshes);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*	pAIMesh = m_pAIScene->mMeshes[i];

		CMeshContainer*	pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, pAIMesh, this, XMLoadFloat4x4(&m_PivotMatrix));
		if (pMeshContainer == nullptr)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Create_Materials(const char* pModelFilePath)
{
	// Material Creation
	/*
	Read the texture material information to be mapped to each mesh constituting this model.
	*/
	if (m_pAIScene == nullptr)
		return E_FAIL;

	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial*	pAIMaterial = m_pAIScene->mMaterials[i];

		MODELMATERIAL ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATERIAL));

		/*
		In a Material there can be up to 18 (AI_TEXTURE_TYPE_MAX) Texture Maps describing that Material.
		(Diffuse Map, Normal Map, Specular Map, Occlusion Map, ..)
		*/
		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			// The Diffuse Map is the second Texture(j = 1)

			aiString strPath;
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char szName[MAX_PATH] = "";
			char szExt[MAX_PATH] = ".dds";	// Forced *.dds Extension (..really bad, but there is no other option).
											// (Map 3ds Max Project is already made with Models exported with *.png Textures, changing all the Textures to *.dds for every Mesh would take forever).
			char szTextureFileName[MAX_PATH] = "";
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

			strcpy_s(szTextureFileName, szName);
			strcat_s(szTextureFileName, szExt);

			char szDirectory[MAX_PATH] = "";
			char szFullPath[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			strcpy_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szTextureFileName);

			_tchar szRealPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szRealPath, MAX_PATH);

			ModelMaterial.pMatTextures[j] = CTexture::Create(m_pDevice, m_pContext, szRealPath);
			if (nullptr == ModelMaterial.pMatTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent)
{
	CHierarchyNode* pHierarchyNode = CHierarchyNode::Create(pNode, pParent);
	if (pHierarchyNode == nullptr)
		return E_FAIL;

	m_Bones.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode);

	return S_OK;
}

HRESULT CModel::Create_Animations()
{
	if (m_pAIScene == nullptr)
		return E_FAIL;

	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*	pAnimation = CAnimation::Create(this, pAIAnimation);
		if (pAIAnimation == nullptr)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PivotMatrix)))
	{
		ERR_MSG(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& pBoneNode : m_Bones)
		Safe_Release(pBoneNode);
	m_Bones.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pMatTextures[i]);
	}
	m_Materials.clear();

	m_Importer.FreeScene();
}