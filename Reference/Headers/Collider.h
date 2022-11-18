#pragma once

#include "Component.h"

BEGIN(Engine)
class CHierarchyNode;
class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
	enum AIM { AIM_DAMAGE_INPUT, AIM_DAMAGE_OUTPUT, AIM_END };
	enum BOUNDING { BOUNDING_ORIGINAL, BOUNDING_WORLD, BOUNDING_END };

public:
	typedef struct tagColliderDesc
	{
		AIM eAim;
		_float3	vScale;
		_float3	vRotation;
		_bool m_bIsAttachedToBone = false;

		// For Bone Non-Attached Colliders
		_float3	vPosition;

		// For Bone Attached Colliders
		CHierarchyNode* pSocket = nullptr;
		_float4x4 pPivotMatrix;
	}COLLIDERDESC;

	typedef struct tagOBB
	{
		_float3	vCenter;
		_float3	vAlignAxis[3];
		_float3	vCenterAxis[3];
	}OBBDESC;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE Get_Type() { return m_eType; }
	COLLIDERDESC Get_ColliderDesc() { return m_ColliderDesc; }
	BoundingBox* Get_AABB() { return m_pAABB[BOUNDING_WORLD]; }
	BoundingOrientedBox* Get_OBB() { return m_pOBB[BOUNDING_WORLD]; }
	BoundingSphere* Get_Sphere() { return m_pSphere[BOUNDING_WORLD]; }

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg);

public:
	void Update(_fmatrix WorldMatrix);
	HRESULT Render();

public:
	_bool Collision(class CCollider* pTargetCollider);

	// Example of Collision functions implemented without using DirectX pre-made Collision functions.
	// (Made to know what happens behind the scene, in case of Interview question).
	/*_bool Collision_AABB(class CCollider* pTargetCollider);
	_bool Collision_OBB(class CCollider* pTargetCollider);*/

private:
	_matrix Remove_Rotation(_fmatrix Matrix);

	// Example Util functions.
	/*_float3 Compute_Min();
	_float3 Compute_Max();
	OBBDESC Compute_OBBDesc();*/

private:
	TYPE m_eType = TYPE_END;
	COLLIDERDESC m_ColliderDesc;

	BoundingBox* m_pAABB[BOUNDING_END] = { nullptr };
	BoundingOrientedBox* m_pOBB[BOUNDING_END] = { nullptr };
	BoundingSphere* m_pSphere[BOUNDING_END] = { nullptr };

	_bool m_isCollision = false;

#ifdef _DEBUG
private:
	ID3D11InputLayout* m_pInputLayout = nullptr;
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
#endif // _DEBUG

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END