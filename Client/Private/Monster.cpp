#include "stdafx.h"

#include "Monster.h"
#include "GameInstance.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CCharacter(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CMonster::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_MONSTER, this);		

	return iEvent;
}

_uint CMonster::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	return iEvent;
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster::Free()
{
	__super::Free();
}