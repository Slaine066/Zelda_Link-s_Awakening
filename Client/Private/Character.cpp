#include "stdafx.h"

#include "Character.h"

CCharacter::CCharacter(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CActor(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter & rhs)
	: CActor(rhs),
	m_tStats(rhs.m_tStats)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_tStats, sizeof(STATS));

	return S_OK;
}

HRESULT CCharacter::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CCharacter::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	return OBJ_NOEVENT;
}

void CCharacter::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}