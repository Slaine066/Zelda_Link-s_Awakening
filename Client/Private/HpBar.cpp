#include "stdafx.h"

#include "HpBar.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Heart.h"

CHpBar::CHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CHpBar::CHpBar(const CHpBar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHpBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	return E_FAIL;

	Init_Hearts();

	return S_OK;
}

_uint CHpBar::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	Compute_Hearts();

	for (auto& pHeart : m_Hearts)
		pHeart->Tick(fTimeDelta);

	return S_OK;
}

_uint CHpBar::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	for (auto& pHeart : m_Hearts)
		pHeart->Late_Tick(fTimeDelta);

	return S_OK;
}

HRESULT CHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	for (auto& pHeart : m_Hearts)
		pHeart->Render();

	return S_OK;
}

void CHpBar::Get_PlayerHp()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	CPlayer* pPlayer = (CPlayer*)pGameInstance->Find_Object(pGameInstance->Get_NextLevelIndex(), TEXT("Layer_Player"));
	m_fMaxHp = pPlayer->Get_Stats().m_fMaxHp;
	m_fCurrentHp = pPlayer->Get_Stats().m_fCurrentHp;
	
	RELEASE_INSTANCE(CGameInstance);
}

void CHpBar::Init_Hearts()
{
	Get_PlayerHp();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UIDESC tUIDesc;
	ZeroMemory(&tUIDesc, sizeof(CUI::UIDESC));

	tUIDesc.m_fSizeX = 55;
	tUIDesc.m_fSizeY = 55;

	for (_uint i = 0; i < m_fMaxHp; i++)
	{
		tUIDesc.m_fX = tUIDesc.m_fSizeX * (i == 0 ? 1 : i + 1);
		tUIDesc.m_fY = tUIDesc.m_fSizeY;
		wcscpy_s(tUIDesc.m_pTextureName, MAX_PATH, TEXT("Prototype_Component_Texture_Heart"));

		CUI* pHeart = (CUI*)pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_UI_Heart"), &tUIDesc);

		m_Hearts.push_back(pHeart);
		((CUI_Heart*)pHeart)->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CHpBar::Compute_Hearts()
{
	Get_PlayerHp();

	for (_float i = 0; i < m_fMaxHp; i += 1)
	{
		if (m_fCurrentHp > (i + .5f))
			((CUI_Heart*)m_Hearts[i])->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_FULL);
		else if (m_fCurrentHp == (i + .5f))
			((CUI_Heart*)m_Hearts[i])->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_HALF);
		else
			((CUI_Heart*)m_Hearts[i])->Set_HeartType(CUI_Heart::HEART_TYPE::HEART_EMPTY);
	}
}

CHpBar* CHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHpBar* pInstance = new CHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Create: CHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHpBar::Clone(void * pArg)
{
	CHpBar* pInstance = new CHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Clone: CHpBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHpBar::Free()
{
	__super::Free();

	for (auto& pHeart : m_Hearts)
		Safe_Release(pHeart);

	m_Hearts.clear();
}