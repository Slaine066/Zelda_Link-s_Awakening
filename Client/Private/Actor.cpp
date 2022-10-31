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

void CActor::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}