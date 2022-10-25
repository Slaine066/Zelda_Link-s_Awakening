#pragma once

#include "Base.h"

/* 게임객체들의 부모가되는 클래스다. */

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	_tchar* Get_ObjName() { return m_pObjName; }
	_tchar* Get_ObjId() { return m_pObjId; }
	_float Get_CamDistance() const { return m_fCamDistance; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Find_Component(const _tchar* pComponentTag);
	virtual bool Picking(_float3& OutPos) {	return false; }

protected:
	HRESULT Add_Components(const _tchar* pComponentTag, _uint iLevelIndex, const _tchar* pPrototypeTag, CComponent** ppOut, void* pArg = nullptr);
	void Compute_CamDistance(_float3 vWorldPos);

protected:
	ID3D11Device* m_pDevice = nullptr; 
	ID3D11DeviceContext* m_pContext = nullptr;

	_tchar m_pObjName[MAX_PATH];	// "Fiona"
	_tchar m_pObjId[MAX_PATH];		// "Fiona_1"
	map<const _tchar*, class CComponent*> m_Components;

	float m_fCamDistance = 0.f;

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};
END