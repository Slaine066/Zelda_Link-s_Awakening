#include "stdafx.h"

#include "TriggerBox_Dynamic.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Player.h"
#include "UI_Manager.h"

CTriggerBox_Dynamic::CTriggerBox_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerBox(pDevice, pContext)
{
}

CTriggerBox_Dynamic::CTriggerBox_Dynamic(const CTriggerBox_Dynamic & rhs)
	: CTriggerBox(rhs)
{
}

HRESULT CTriggerBox_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTriggerBox_Dynamic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_float fTriggerBoxScale = XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_tTriggerBoxDesc.mWorldMatrix).r[0]));
	_float3 vTriggerBoxPosition = *(_float3*)&m_tTriggerBoxDesc.mWorldMatrix.m[3];
	vTriggerBoxPosition.y += fTriggerBoxScale / 2;

	CCollider::COLLIDERDESC	ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.eAim = CCollider::AIM::AIM_TRIGGER_BOX;
	ColliderDesc.vScale = _float3(fTriggerBoxScale, fTriggerBoxScale, fTriggerBoxScale);
	ColliderDesc.vPosition = vTriggerBoxPosition;

	/* For.Com_Collider*/
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pCollider, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CTriggerBox_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_GROUP::COLLISION_TRIGGER_BOX, this);

	return OBJ_NOEVENT;
}

_uint CTriggerBox_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	/* Should not run on the FirstFrame after spawn, to give other Objects time to update. 
	(Bleah.. should remove Get_Position() function ¤Ð¤Ð) */
	if (m_bIsFirstFrame)
	{
		m_bIsFirstFrame = false;
		return OBJ_NOEVENT;
	}

	CGameObject* pCollidedObject = nullptr;
	_bool bIsCollided = CGameInstance::Get_Instance()->Collision_with_Group(CCollision_Manager::COLLISION_GROUP::COLLISION_PLAYER, m_pCollider, CCollider::AIM::AIM_DAMAGE_INPUT, pCollidedObject);
	
	if (!strcmp(m_tTriggerBoxDesc.pTriggerName, CGameInstance::Get_Instance()->Get_SpawnTriggerBoxName()))
	{
		if (bIsCollided)
		{
			_bool bIsJustSpawned = CGameInstance::Get_Instance()->Get_IsJustSpawned();
			if (bIsJustSpawned)
				return OBJ_NOEVENT;

			switch (CGameInstance::Get_Instance()->Get_CurrentLevelIndex())
			{
			case LEVEL_FIELD:
				Field_Triggers(fTimeDelta);
				break;
			case LEVEL_MORIBLINCAVE:
				MoriblinCave_Triggers(fTimeDelta);
				break;
			case LEVEL_BOTTLEGROTTO:
				BottleGrotto_Triggers(fTimeDelta);
				break;
			case LEVEL_MARINHOUSE:
				MarinHouse_Triggers(fTimeDelta);
				break;
			}

			return OBJ_DESTROY;
		}
		else
		{
			CGameInstance::Get_Instance()->Set_IsJustSpawned(false);
			CGameInstance::Get_Instance()->Set_SpawnTriggerBox(nullptr);
		}
	}
	else
	{
		if (bIsCollided)
		{
			switch (CGameInstance::Get_Instance()->Get_CurrentLevelIndex())
			{
			case LEVEL_FIELD:
				Field_Triggers(fTimeDelta);
				break;
			case LEVEL_MORIBLINCAVE:
				MoriblinCave_Triggers(fTimeDelta);
				break;
			case LEVEL_BOTTLEGROTTO:
				BottleGrotto_Triggers(fTimeDelta);
				break;
			case LEVEL_MARINHOUSE:
				MarinHouse_Triggers(fTimeDelta);
				break;
			}

			return OBJ_DESTROY;
		}
	}

	return OBJ_NOEVENT;
}

HRESULT CTriggerBox_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CTriggerBox_Dynamic::Field_Triggers(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!strcmp(m_tTriggerBoxDesc.pTriggerName, "MoriblinCave_Entrance"))
	{
		if (m_fTimer < .5f)
		{
			if (m_fTimer == 0)
				CUI_Manager::Get_Instance()->ScreenFadeOut();

			m_fTimer += fTimeDelta;
		}
		else
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MORIBLINCAVE))))
				return;

			pGameInstance->Set_SpawnTriggerBox("MoriblinCave_Entrance");
			pGameInstance->Set_IsJustSpawned(true);
			pGameInstance->ClearLights();

			CUI_Manager::Get_Instance()->Clear();
			
			m_fTimer = 0.f;
		}
	}
	else if (!strcmp(m_tTriggerBoxDesc.pTriggerName, "MarinHouse_Entrance"))
	{
		if (m_fTimer < .5f)
		{
			if (m_fTimer == 0)
				CUI_Manager::Get_Instance()->ScreenFadeOut();

			m_fTimer += fTimeDelta;
		}
		else
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MARINHOUSE))))
				return;

			pGameInstance->PlaySounds(TEXT("Enter_Door.wav"), SOUND_GROUND, 1.f);
			pGameInstance->Set_SpawnTriggerBox("MarinHouse_Entrance");
			pGameInstance->Set_IsJustSpawned(true);
			pGameInstance->ClearLights();

			CUI_Manager::Get_Instance()->Clear();

			m_fTimer = 0.f;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CTriggerBox_Dynamic::MoriblinCave_Triggers(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!strcmp(m_tTriggerBoxDesc.pTriggerName, "MoriblinCave_Entrance"))
	{
		if (m_fTimer < .5f)
		{
			if (m_fTimer == 0)
				CUI_Manager::Get_Instance()->ScreenFadeOut();

			m_fTimer += fTimeDelta;
		}
		else
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIELD))))
				return;

			pGameInstance->Set_SpawnTriggerBox("MoriblinCave_Entrance");
			pGameInstance->Set_IsJustSpawned(true);
			pGameInstance->ClearLights();

			CUI_Manager::Get_Instance()->Clear();

			m_fTimer = 0.f;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CTriggerBox_Dynamic::BottleGrotto_Triggers(_float fTimeDelta)
{
}

void CTriggerBox_Dynamic::MarinHouse_Triggers(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!strcmp(m_tTriggerBoxDesc.pTriggerName, "MarinHouse_Entrance"))
	{
		if (m_fTimer < .5f)
		{
			if (m_fTimer == 0)
				CUI_Manager::Get_Instance()->ScreenFadeOut();

			m_fTimer += fTimeDelta;
		}
		else
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FIELD))))
				return;

			pGameInstance->PlaySounds(TEXT("Enter_Door.wav"), SOUND_GROUND, 1.f);
			pGameInstance->Set_SpawnTriggerBox("MarinHouse_Entrance");
			pGameInstance->Set_IsJustSpawned(true);
			pGameInstance->ClearLights();

			CUI_Manager::Get_Instance()->Clear();

			m_fTimer = 0.f;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

CTriggerBox_Dynamic * CTriggerBox_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerBox_Dynamic* pInstance = new CTriggerBox_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CTriggerBox_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTriggerBox_Dynamic::Clone(void* pArg)
{
	CTriggerBox_Dynamic* pInstance = new CTriggerBox_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CTriggerBox_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerBox_Dynamic::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
}