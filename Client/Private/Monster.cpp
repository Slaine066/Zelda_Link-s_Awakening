#include "stdafx.h"

#include "Monster.h"
#include "GameInstance.h"
#include "Item.h"

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

void CMonster::DropItems()
{
	CItem::ITEMDESC tItemDesc;
	ZeroMemory(&tItemDesc, sizeof(CItem::ITEMDESC));
	tItemDesc.m_eItemType = CItem::ITEMTYPE::TYPE_DROP;
	tItemDesc.mWorldMatrix = m_pTransformCom->Get_World4x4();
	
	_uint iRand = rand() % (ITEMID::ITEM_BOMB + 1 * 3); /* Multiply by 3 so that the droprate is 33%. */
	switch ((ITEMID)iRand)
	{
		case ITEMID::ITEM_RUPEE_GREEN:
		case ITEMID::ITEM_BOMB:
			tItemDesc.m_eItemId = (ITEMID)iRand;
			break;
		default:
			return; /* Just return if out of Item Drop range. */
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Item"), TEXT("Prototype_GameObject_Item"), pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Item"), &tItemDesc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CMonster::Free()
{
	__super::Free();
}