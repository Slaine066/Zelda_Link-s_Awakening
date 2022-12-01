#include "stdafx.h"

#include "FallingTile.h"
#include "GameInstance.h"

CFallingTile::CFallingTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CInteractableObject(pDevice, pContext)
{
}

CFallingTile::CFallingTile(const CFallingTile & rhs)
	: CInteractableObject(rhs)
{
}

HRESULT CFallingTile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFallingTile::Initialize(void * pArg)
{
	/*if (FAILED(__super::Initialize(pArg)))
	return E_FAIL;*/

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CFallingTile::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	if (m_bShouldDestroy)
		return OBJ_DESTROY;

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_OBJECT, this);

	return OBJ_NOEVENT;
}

_uint CFallingTile::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return OBJ_NOEVENT;
}

HRESULT CFallingTile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	Render_Colliders();

	return S_OK;
}

HRESULT CFallingTile::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFallingTile::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

CFallingTile* CFallingTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFallingTile* pInstance = new CFallingTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CFallingTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFallingTile::Clone(void * pArg)
{
	CFallingTile* pInstance = new CFallingTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CFallingTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFallingTile::Free()
{
	__super::Free();
}