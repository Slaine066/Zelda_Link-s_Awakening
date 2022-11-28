#include "TriggerBox.h"

CTriggerBox::CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CTriggerBox::CTriggerBox(const CTriggerBox & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTriggerBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTriggerBox::Initialize(void * pArg)
{
	memcpy(&m_tTriggerBoxDesc, pArg, sizeof(TRIGGERBOXDESC));

	return S_OK;
}

_uint CTriggerBox::Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

_uint CTriggerBox::Late_Tick(_float fTimeDelta)
{
	return OBJ_NOEVENT;
}

HRESULT CTriggerBox::Render()
{
	m_pCollider->Render();

	return S_OK;
}

void CTriggerBox::Free()
{
	__super::Free();
}