#include "Layer.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_Component(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Find_Component(pComponentTag);	
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(const _tchar* pObjName, CGameObject * pGameObject, const _tchar* pLayerTag)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	// Create ObjectId ("Fiona" > "Fiona_1", "Fiona_2")
	if (pObjName)
	{
		wcscpy_s(pGameObject->Get_ObjName(), MAX_PATH, pObjName);
		wcscpy_s(pGameObject->Get_ObjId(), MAX_PATH, pObjName);
		wcscpy_s(pGameObject->Get_LayerId(), MAX_PATH, pLayerTag);

		_uint iCounter = 0;
		for (auto& pObj : m_GameObjects)
		{
			if (!wcscmp(pObj->Get_ObjName(), pObjName))
				iCounter++;
		}

		if (iCounter) // > 0
		{
			wcscat_s((_tchar*)pGameObject->Get_ObjId(), MAX_PATH, TEXT("_"));

			wstring sCounter = to_wstring(iCounter);
			wcscat_s((_tchar*)pGameObject->Get_ObjId(), MAX_PATH, sCounter.c_str());
		}
	}

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			int iEvent = pGameObject->Tick(fTimeDelta);
			if (iEvent == OBJ_DEAD)
			{
				Safe_Release(pGameObject);
			}
		}
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(fTimeDelta);
	}
}

CGameObject * CLayer::Get_Object(_uint iIndex)
{
	if (m_GameObjects.size() <= iIndex)
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

void CLayer::Remove_Object(CGameObject* pGameObj)
{
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&](CGameObject* rObj) {
		return !wcscmp(rObj->Get_ObjId(), pGameObj->Get_ObjId());
	});	
	
	if (iter == m_GameObjects.end())
		return;

	m_GameObjects.erase(iter);
	Safe_Release(pGameObj);
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}