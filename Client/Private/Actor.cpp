#include "stdafx.h"

#include "Actor.h"
#include "GameInstance.h"

CActor::CActor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CActor::CActor(const CActor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CActor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CActor::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

_uint CActor::Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

void CActor::Late_Tick(_float fTimeDelta)
{
}

HRESULT CActor::Render()
{
	if (m_pShaderCom == nullptr || m_pModelCom == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CActor::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}