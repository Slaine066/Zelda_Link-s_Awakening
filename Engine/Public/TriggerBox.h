#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class ENGINE_DLL CTriggerBox abstract : public CGameObject
{
public:
	typedef struct tagTriggerBoxDescription
	{
		char pTriggerName[MAX_PATH];
		_float4x4 mWorldMatrix;
		BoundingBox pTriggerBox;
	} TRIGGERBOXDESC;

protected:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTriggerBox(const CTriggerBox& rhs);
	virtual ~CTriggerBox() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual _uint Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	TRIGGERBOXDESC m_tTriggerBoxDesc;
	CCollider* m_pCollider = nullptr;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};
END