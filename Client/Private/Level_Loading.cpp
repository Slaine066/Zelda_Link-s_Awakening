#include "stdafx.h"

#include "Level_Loading.h"
#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_Field.h"
#include "Level_MoriblinCave.h"
#include "Level_BottleGrotto.h"
#include "TriggerBox.h"
#include "Player.h"
#include "UIManager.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	if (m_pLoader->Get_Finished())
	{
		CLevel* pNewLevel = nullptr;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pGameInstance->Set_NextLevel(m_eNextLevel);
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_FIELD:
			pGameInstance->Set_NextLevel(m_eNextLevel);
			pNewLevel = CLevel_Field::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_MORIBLINCAVE:
			pGameInstance->Set_NextLevel(m_eNextLevel);
			pNewLevel = CLevel_MoriblinCave::Create(m_pDevice, m_pContext);
			break;
		/*case LEVEL_BOTTLEGROTTO:
			pNewLevel = CLevel_BottleGrotto::Create(m_pDevice, m_pContext);
			break;*/
		}

		if (!pNewLevel)
			return;

		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
			return;

		Compute_SpawnPosition(fTimeDelta);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CLevel_Loading::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
}

void CLevel_Loading::Compute_SpawnPosition(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	char* pSpawnTriggerBoxName = pGameInstance->Get_SpawnTriggerBoxName();
	if (pSpawnTriggerBoxName[0] == 0)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTriggerBox* pSpawnTriggerBox = pGameInstance->Get_TriggerBox(pSpawnTriggerBoxName);
	pSpawnTriggerBox->Set_IsFirstFrame(true);

	_float3 vSpawnPosition = (_float3)pSpawnTriggerBox->Get_TriggerBoxDesc().mWorldMatrix.m[3];

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Player"));
	pPlayer->Get_Transform()->Set_State(CTransform::STATE::STATE_TRANSLATION, XMVectorSetW(XMLoadFloat3(&vSpawnPosition), 1.f));
	pPlayer->Get_Navigation()->Set_InitialPosition(vSpawnPosition);
	pPlayer->Get_Navigation()->Compute_CurrentCell();

	RELEASE_INSTANCE(CGameInstance);
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Create: CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
