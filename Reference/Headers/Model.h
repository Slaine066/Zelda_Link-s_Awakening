#pragma once

#include "Component.h"
#include "HierarchyNode.h"

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
	_float4x4 Get_PivotFloat4x4() const { return m_PivotMatrix; }

	/* For.Meshes */
	char* Get_MeshName(_uint iMeshIndex);
	_uint Get_NumMeshContainers() const { return m_iNumMeshes; }
	vector<class CMeshContainer*> Get_MeshContainers() const { return m_Meshes; }

	/* For.Bones*/
	class CHierarchyNode* Get_BonePtr(const char* pBoneName) const;

	/* For.Animations */
	char* Get_AnimationName(_uint iAnimationIndex);
	char* Get_CurrentAnimationName();
	_uint Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }
	void Set_CurrentAnimIndex(_uint iAnimIndex) { m_iCurrentAnimIndex = iAnimIndex; }
	_bool Is_Keyframe(char* pChannelName, _uint iKeyframe);
	_bool Under_Keyframe(char* pChannelName, _uint iKeyframe);
	_bool Over_Keyframe(char* pChannelName, _uint iKeyframe);
	_bool Between_Keyframe(char * pChannelName, _uint iKeyframeLower, _uint iKeyframeUpper);
	void Reset_CurrentAnimation();

public:
	HRESULT SetUp_Material(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Play_Animation(_float fTimeDelta, OUT _bool& bIsFinished, _bool bIsLoop = true);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0, _bool bAnimate = true);

private:
	const aiScene* m_pAIScene = nullptr;
	Assimp::Importer m_Importer;

private:
	TYPE							m_eModelType = TYPE_END;

	// Meshes Variables
	_uint							m_iNumMeshes = 0;
	vector<class CMeshContainer*>	m_Meshes;

	// Materials Variables
	_uint							m_iNumMaterials = 0;
	vector<MODELMATERIAL>			m_Materials;

	// Bones Variables
	_uint							m_iNumBones;
	vector<class CHierarchyNode*>	m_Bones;

	// Animations Variables
	_uint							m_iNumAnimations = 0;
	vector<class CAnimation*>		m_Animations;
	_uint							m_iCurrentAnimIndex = 0;

	_float4x4						m_PivotMatrix;

private:
	HRESULT Create_MeshContainer();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Create_HierarchyNodes(const aiNode* pNode, CHierarchyNode* pParent = nullptr);
	HRESULT Create_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eModelType, const char* pModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END