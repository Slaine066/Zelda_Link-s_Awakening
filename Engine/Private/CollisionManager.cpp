#include "CollisionManager.h"
#include "Component.h"
#include "Transform.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	if (!pGameObject)
		return E_FAIL;

	m_GameObjects[eCollisionGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

void CCollision_Manager::Remove_CollisionGroup(COLLISION_GROUP eCollisionGroup, CGameObject * pGameObject)
{
	auto& iter = m_GameObjects[eCollisionGroup].begin();

	while (iter != m_GameObjects[eCollisionGroup].end())
	{
		if (*iter == pGameObject)
			iter = m_GameObjects[eCollisionGroup].erase(iter);
		else
			++iter;
	}
}

_bool CCollision_Manager::Collision_with_Group(COLLISION_GROUP eGroup, CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT CGameObject& pDamagedObject)
{
	if (!pDamageCauser)
		return false;

	for (auto& iter : m_GameObjects[eGroup])
	{
		/*if (nullptr != iter && CGameInstance::Get_Instance()->Is_In_Frustum(iter->Get_Position(), iter->Get_Radius()) == true)
		{*/
			CCollider* pDamaged = iter->Get_Collider(eCollisionAim);
			if (!pDamaged)
				continue;

			if (pDamageCauser->Collision(pDamaged))
			{
				pDamagedObject = *iter;
				return true;
			}
		/*}*/
	}

	return false;
}

_bool CCollision_Manager::Collision_Check_Group_Multi(COLLISION_GROUP eGroup, CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT vector<CGameObject*>& pDamagedObjects)
{
	if (!pDamageCauser)
		return false;

	for (auto& iter : m_GameObjects[eGroup])
	{
		/*if (nullptr != iter && CGameInstance::Get_Instance()->Is_In_Frustum(iter->Get_Position(), iter->Get_Radius()) == true)
		{*/
			CCollider* pDamaged = iter->Get_Collider(eCollisionAim);
			if (!pDamaged)
				continue;

			if (pDamageCauser->Collision(pDamaged))
				pDamagedObjects.push_back(iter);
		/*}*/
	}

	if (!pDamagedObjects.empty())
		return true;
	else 
		return false;

	return false;
}

HRESULT CCollision_Manager::Reset_ColliderGroup()
{
	for (_uint i = 0; i < COLLISION_GROUPEND; ++i)
	{
		for (auto& pGameObject : m_GameObjects[i])
		{
			if (nullptr != pGameObject)
			{
				Safe_Release(pGameObject);
			}
		}
		m_GameObjects[i].clear();
	}

	return S_OK;
}

void CCollision_Manager::Free()
{
	for (_uint i = 0; i < COLLISION_GROUPEND; ++i)
		m_GameObjects[i].clear();
}