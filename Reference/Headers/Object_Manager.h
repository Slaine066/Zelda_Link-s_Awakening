#pragma once

#include "Base.h"

/*  생성된 게임오브젝트들을 모아서 보관한다. : 사용자가 나누고 싶은 기준에 따라. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)

public:
	typedef map<const _tchar*, class CLayer*> LAYERS;

private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	class CGameObject* Get_Object(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex = 0);
	LAYERS Get_Layers(_uint iLevelIndex);
	class CLayer* Get_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	HRESULT Reserve_Container(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pObjName, const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_Out(const _tchar* pObjName, const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, OUT CGameObject*& pGameObjectOut, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Remove_GameObject(CGameObject* pGameObj, _uint iLevelIndex, const _tchar* pLayerTag);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

private:
	map<const _tchar*, class CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, class CGameObject*>	PROTOTYPES;

private: /* 내가 구분하고 싶은 대로 그룹을 지어 레벨별로 객체를 추가한다. */	
	_uint										m_iNumLevels = 0;
	map<const _tchar*, class CLayer*>*			m_pLayers = nullptr;

private:
	class CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};
END