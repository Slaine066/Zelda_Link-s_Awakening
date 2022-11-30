#pragma once

#include "Base.h"
#include "GameObject.h"

/* Collision Manager Class
This Singleton stores a list with all the Collidable Objects, which every frames:
	- Gets filled with Objects;	>	When calling "Add_CollisionGroup()" at the beginning of the "Tick()" function by every Objects;
	- Check Collisions;			>	When calling "Collision_with_Group" and "Collision_Check_Group_Multi" when needed;
	- Gets emptied.				>	When calling "Reset_ColliderGroup"at the end of the "Tick_Engine()" function.
*/

BEGIN(Engine)
class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)

public:
	enum COLLISION_GROUP { COLLISION_PLAYER, COLLISION_MONSTER, COLLISION_TRIGGER_BOX, COLLISION_OBJECT, COLLISION_GROUPEND };

private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	// Add Object to Collision Group
	HRESULT Add_CollisionGroup(COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);
	// Remove Object from Collision Group
	void Remove_CollisionGroup(COLLISION_GROUP eCollisionGroup, class CGameObject* pGameObject);

	/* Collision Functions */
	// Collision between Object and first Collided Object (1:1 Collision)
	_bool Collision_with_Group(COLLISION_GROUP eGroup, class CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT CGameObject* pDamagedObject);
	// Collision between Object and all Collided Object (1:N Collisions) - Preferrable to use 
	_bool Collision_Check_Group_Multi(COLLISION_GROUP eGroup, class CCollider* pDamageCauser, CCollider::AIM eCollisionAim, OUT vector<CGameObject*>& pDamagedObjects);

	// Empty the Collidable Objects List (which gets refilled at the beginning of every frame)
	HRESULT Reset_ColliderGroup();

private:
	list<class CGameObject*> m_GameObjects[COLLISION_GROUPEND];
	typedef list<class CGameObject*> GAMEOBJECTS;

public:
	virtual void Free() override;
};
END