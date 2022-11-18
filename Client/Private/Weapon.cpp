#include "stdafx.h"

#include "Weapon.h"
#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CActor(rhs)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_RIGHT, .4f);
	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_UP, .4f);
	m_pTransformCom->Set_Scale(CTransform::STATE::STATE_LOOK, .4f);

	/* Revert PivotMatrix. */
	m_pTransformCom->Set_RotationY(180.f);

	return S_OK;
}

_uint CWeapon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// Collision Handling
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Add_CollisionGroup(m_WeaponDesc.bIsPlayerWeapon ? CCollision_Manager::COLLISION_PLAYER : CCollision_Manager::COLLISION_MONSTER, this);

	// Assemble CombinedWorldMatrix
	_matrix	SocketMatrix = m_WeaponDesc.pSocket->Get_CombinedTransformationMatrix() * XMLoadFloat4x4(&m_WeaponDesc.SocketPivotMatrix) * XMLoadFloat4x4(m_WeaponDesc.pParentWorldMatrix);
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

	return OBJ_NOEVENT;
}

void CWeapon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWeapon::Render()
{
	__super::Render();

	_uint iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	// Only in Debug
	Render_Colliders();

	return S_OK;
}

HRESULT CWeapon::Ready_Components(void * pArg)
{
	if (pArg)
	{
		ZeroMemory(&m_WeaponDesc, sizeof(WEAPONDESC));
		memcpy(&m_WeaponDesc, pArg, sizeof(WEAPONDESC));
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	XMStoreFloat4x4(&TransformDesc.vInitialWorldMatrix, XMMatrixIdentity());

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, m_WeaponDesc.pModelPrototypeId, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_vCollidersCom.resize(1); // Numbers of Colliders needed for this Object

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	memcpy(&ColliderDesc, &m_WeaponDesc.tColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_vCollidersCom[0], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::SetUp_ShaderResources()
{
	if (!m_pShaderCom)
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_CombinedWorldMatrix)));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CWeapon::Update_Colliders()
{
	for (auto& pCollider : m_vCollidersCom)
		if (pCollider)
			pCollider->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

CWeapon * CWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeapon::Clone(void * pArg)
{
	CWeapon* pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CWeapon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_WeaponDesc.pSocket);
}