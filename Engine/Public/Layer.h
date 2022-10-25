#pragma once

#include "GameObject.h"

/* 객체들을 모아 놓는다. */
BEGIN(Engine)
class CLayer final : public CBase
{
public:
	typedef list<class CGameObject*> GAMEOBJECTS;

private:
	CLayer();
	virtual ~CLayer() = default;

public:
	GAMEOBJECTS Get_Objects() { return m_GameObjects; }
	class CComponent* Get_Component(const _tchar* pComponentTag, _uint iIndex = 0);

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(const _tchar* pObjName, class CGameObject* pGameObject);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	class CGameObject* Get_Object(_uint iIndex = 0);
	void Remove_Object(CGameObject* pGameObj);

private:
	list<class CGameObject*> m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};
END