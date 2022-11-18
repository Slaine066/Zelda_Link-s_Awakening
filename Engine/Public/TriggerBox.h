#pragma once

#include "GameObject.h"
#include "Renderer.h"

BEGIN(Engine)
class ENGINE_DLL CTriggerBox final : public CGameObject
{
public:
	typedef struct tagTriggerDesc
	{
		_float4x4 mWorldMatrix;
		CCollider* pTargetCollider;
		void(*TriggerCallback)();
	}TRIGGERDESC;

private:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerBox(const CTriggerBox& rhs);
	virtual ~CTriggerBox() = default;

public:
	TRIGGERDESC Get_TriggerDesc() { return m_tTriggerDesc; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;

public:
	HRESULT Render();
	_bool Collision(class CCollider* pTargetCollider);

private:
	TRIGGERDESC m_tTriggerDesc;
	BoundingBox* m_pTriggerBox;

	_bool m_isCollision = false;

#ifdef _DEBUG
private:
	ID3D11InputLayout* m_pInputLayout = nullptr;
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
#endif // _DEBUG

public:
	static CTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END