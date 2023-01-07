#include "stdafx.h"

#include "Npc.h"
#include "GameInstance.h"

CNpc::CNpc(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCharacter(pDevice, pContext)
{
}

CNpc::CNpc(const CNpc & rhs)
	: CCharacter(rhs)
{
}

HRESULT CNpc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CNpc::Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Tick(fTimeDelta);

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	if (iEvent == OBJ_STOP)
		return iEvent;

	return iEvent;
}

_uint CNpc::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	return iEvent;
}

HRESULT CNpc::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpc::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	return S_OK;
}

void CNpc::Free()
{
	__super::Free();
}