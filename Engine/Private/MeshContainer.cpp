#include "MeshContainer.h"
#include "HierarchyNode.h"
#include "Picking.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)
	, m_pAIMesh(rhs.m_pAIMesh)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
{
	strcpy_s(m_szName, rhs.m_szName);
}

void CMeshContainer::Get_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix)
{
	if (m_iNumBones == 0)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	_uint iNumBones = 0;

	/* Offset * Combined * Pivot */
	for (auto& pBoneNode : m_Bones)
		XMStoreFloat4x4(&pBoneMatrices[iNumBones++], (pBoneNode->Get_OffsetMatrix() * pBoneNode->Get_CombinedTransformationMatrix() * PivotMatrix));
}

HRESULT CMeshContainer::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	// Store Mesh Name
	/* 
	For finding the Bone to which attach a Mesh without Bones.
	In case a Mesh does not have any bones, it will be placed at the Model origin position.
	(In the Bone Hierarchy there is always a bone named as the Mesh that should be attached to it)
	(Ex: Sword Mesh > There will be a bone in the Bone Hierarchy called "Sword", or similar)
	*/
	strcpy_s(m_szName, pAIMesh->mName.data); 
	
	// Store Material Index 
	/* 
	For finding Material Texture Maps that should be mapped when this Mesh is drawn.
	(The "Mesh : Material" relationship is 1 : 1)
	*/
	m_iMaterialIndex = pAIMesh->mMaterialIndex; 
	
	m_pAIMesh = pAIMesh;

#pragma region Vertices
	HRESULT hr = 0;

	if (eModelType == CModel::TYPE_NONANIM)
		hr = Create_VertexBuffer_NonAnimModel(pAIMesh, PivotMatrix);
	else
		hr = Create_VertexBuffer_AnimModel(pAIMesh, pModel);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion

#pragma region Indices
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iIndicesByte = sizeof(FACEINDICES32);
	m_iNumPrimitive = m_pAIMesh->mNumFaces;
	m_iNumIndicesPerPrimitive = 3;

	m_BufferDesc.ByteWidth = m_iIndicesByte * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* Create a Static Buffer. */
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = sizeof(_ushort);

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		aiFace AIFace = m_pAIMesh->mFaces[i];

		pIndices[i]._0 = AIFace.mIndices[0];
		pIndices[i]._1 = AIFace.mIndices[1];
		pIndices[i]._2 = AIFace.mIndices[2];
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Initialize(void* pArg)
{
	return S_OK;
}

_bool CMeshContainer::Picking(CTransform * pTransform, _float3& pOut)
{
	CPicking* pPicking = GET_INSTANCE(CPicking);
	pPicking->Compute_LocalRayInfo(pTransform);

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];

	// Loop through Mesh Faces (Triangle) to populate Indices
	for (_uint i = 0; i < m_iNumPrimitive; i++)
	{
		aiFace AIFace = m_pAIMesh->mFaces[i];

		pIndices[i]._0 = AIFace.mIndices[0];
		pIndices[i]._1 = AIFace.mIndices[1];
		pIndices[i]._2 = AIFace.mIndices[2];
	}

	// Loop through Mesh Faces (Triangle) to check Intersection
	for (_uint i = 0; i < m_iNumPrimitive; i++)
	{
		_vector vPointA;
		_vector vPointB;
		_vector vPointC;

		memcpy(&vPointA, &m_pAIMesh->mVertices[pIndices[i]._0], sizeof(_float3));
		vPointA = XMVectorSetW(vPointA, 1.f);
		memcpy(&vPointB, &m_pAIMesh->mVertices[pIndices[i]._1], sizeof(_float3));
		vPointB = XMVectorSetW(vPointB, 1.f);
		memcpy(&vPointC, &m_pAIMesh->mVertices[pIndices[i]._2], sizeof(_float3));
		vPointC = XMVectorSetW(vPointC, 1.f);

		if (pPicking->Intersect(vPointA, vPointB, vPointC, pOut))
		{
			// Get Output Position in World Space.
			XMStoreFloat3(&pOut, XMVector3TransformCoord(XMLoadFloat3(&pOut), pTransform->Get_WorldMatrix()));
		
			Safe_Delete_Array(pIndices);
			RELEASE_INSTANCE(CPicking);
			return true;
		}		
	}

	Safe_Delete_Array(pIndices);
	RELEASE_INSTANCE(CPicking);
	return false;
}

HRESULT CMeshContainer::SetUp_Bones(CModel* pModel)
{
	// Loop over Bones making up this Mesh
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*	pAIBone = m_pAIMesh->mBones[i];

		CHierarchyNode*	pBoneNode = pModel->Get_BonePtr(pAIBone->mName.data);
		if (pBoneNode == nullptr)
			return E_FAIL;

		// Add this Bone to the Mesh
		m_Bones.push_back(pBoneNode);
		Safe_AddRef(pBoneNode);

		// Get the OffsetMatrix from the Mesh Bone (aiBone)
		_float4x4 OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		// Set the OffsetMatrix to the Model Bone (aiNode)
		pBoneNode->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
	}

	// If the Mesh does not have any Bones
	if (m_iNumBones == 0)
	{
		// Get the Bone (aiNode) to which this Mesh should be attached to.
		CHierarchyNode*	pNode = pModel->Get_BonePtr(m_szName);
		if (pNode == nullptr)
			return S_OK;

		m_iNumBones = 1;

		// Add this Bone to the Mesh
		m_Bones.push_back(pNode);
		Safe_AddRef(pNode);
	}

	return S_OK;
}

HRESULT CMeshContainer::Create_VertexBuffer_NonAnimModel(const aiMesh * pAIMesh, _fmatrix PivotMatrix)
{
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXMODEL);
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumVertexBuffers = 1;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* Create a Static Buffer. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL* pVertices = new VTXMODEL[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &m_pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &m_pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexUV, &m_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &m_pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMeshContainer::Create_VertexBuffer_AnimModel(const aiMesh * pAIMesh, CModel * pModel)
{
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXANIMMODEL);
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumVertexBuffers = 1;
	m_eFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT; /* Create a Static Buffer. */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL* pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &m_pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &m_pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &m_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &m_pAIMesh->mTangents[i], sizeof(_float3));
	}

	/* Number of Bones affecting this Mesh */
	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/*
		An aiBone has a name by which it can be found in the frame hierarchy and by which it can be addressed by animations. 
		In addition it has a number of influences on vertices. 
		And a matrix relating the mesh position to the position of the bone at the time of binding.
		*/
		aiBone*	pAIBone = pAIMesh->mBones[i];

		// Loop over all the Vertices affected by "i" aiBone
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			// Index of the "j" Vertex affected by by "i" aiBone 
			_uint iVertexIndex = pAIBone->mWeights[j].mVertexId;

			// Same Vertex can be influenced by 4 Bones only.
			// Therefore in the Vertex Structure, BlendWeight and BlendIndex are implemented as a 4 dimension variable (x, y, z, w).
			if (pVertices[iVertexIndex].vBlendWeight.x == 0)
			{
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
				pVertices[iVertexIndex].vBlendIndex.x = i;
			}
			else if (pVertices[iVertexIndex].vBlendWeight.y == 0)
			{
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
				pVertices[iVertexIndex].vBlendIndex.y = i;
			}
			else if (pVertices[iVertexIndex].vBlendWeight.z == 0)
			{
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
				pVertices[iVertexIndex].vBlendIndex.z = i;
			}
			else if (pVertices[iVertexIndex].vBlendWeight.w == 0)
			{
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
				pVertices[iVertexIndex].vBlendIndex.w = i;
			}
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMeshContainer* CMeshContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, pModel, PivotMatrix)))
	{
		ERR_MSG(TEXT("Failed to Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*	pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshContainer::Free()
{
	for (auto& pBoneNode : m_Bones)
		Safe_Release(pBoneNode);

	__super::Free();
}