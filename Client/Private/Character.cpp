#include "stdafx.h"

#include "Character.h"
#include "GameInstance.h"

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
	_uint iEvent = __super::Tick(fTimeDelta);

	return iEvent;
}

_uint CCharacter::Late_Tick(_float fTimeDelta)
{
	_uint iEvent = __super::Late_Tick(fTimeDelta);

	if (iEvent == OBJ_STOP)
		return iEvent;

	Compute_ShaderTimers(fTimeDelta);

	return iEvent;
}

HRESULT CCharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	return S_OK;
}

void CCharacter::Compute_ShaderTimers(_float fTimeDelta)
{
	switch (m_eShaderPass)
	{
	case VTXANIMMODELPASS::VTXANIMMODEL_HIT:
	{
		if (m_fHitTimer > m_fHitLifespan)
			m_eShaderPass = VTXANIMMODELPASS::VTXANIMMODEL_DEFAULT;
		else
			m_fHitTimer += fTimeDelta;
	}
	break;
	case VTXANIMMODELPASS::VTXANIMMODEL_DISSOLVE:
		m_fDissolveTimer += fTimeDelta;
		break;
	case VTXANIMMODELPASS::VTXANIMMODEL_DISSOLVE_BOSS:
		m_fDissolveTimer += fTimeDelta;
		break;
	}
}

void CCharacter::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTextureCom);
}