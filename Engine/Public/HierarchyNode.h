#pragma once

#include "Base.h"

/* Class for each Bone that makes up the Bones Hierarchy. */
BEGIN(Engine)
class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	const char* Get_Name() const { return m_szName; }

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) { XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix); }
	void Set_TransformationMatrix(_fmatrix TransformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix); }

	_matrix Get_OffsetMatrix() const { return XMLoadFloat4x4(&m_OffsetMatrix); }
	_matrix Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }

public:
	HRESULT Initialize(const aiNode* pNode, CHierarchyNode* pParent);
	void Invalidate_CombinedTransformationmatrix();

private:
	char m_szName[MAX_PATH] = "";

	/* Since I needed the parent's m_CombinedTransformationMatrix to create my m_CombinedTransformationMatrix. */
	CHierarchyNode*	m_pParent = nullptr;

	/* The default is an Identity Matrix, filling in values from Mesh Bone (aiBone). */
	_float4x4 m_OffsetMatrix; 

	/* 
	Both the m_TransformationMatrix and m_CombinedTransformationMatrix will change when the animation plays.  
	
	Represents the Transform State of this Bone only (excluding Parent Bone Transform State).
	Transform State is expressed relative to Parent Bone.	
	*/
	_float4x4 m_TransformationMatrix;

	/*
	Represents the Transform State of this Bone including Parent Bone Transform State.
	Transform State is expressed relative to Origin.
	*/
	_float4x4 m_CombinedTransformationMatrix; /* = m_TransformationMatrix * m_pParent->m_CombinedTransformationMatrix */

public:
	static CHierarchyNode* Create(const aiNode* pNode, CHierarchyNode* pParent);
	virtual void Free() override;
};
END