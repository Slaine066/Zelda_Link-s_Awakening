#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CActor abstract : public CGameObject
{
protected:
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& rhs);
	virtual ~CActor() = default;

protected:
	CShader*	m_pShaderCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;

protected:
	virtual HRESULT Ready_Components() = 0;
	virtual HRESULT SetUp_ShaderResources() = 0;

public:
	virtual void Free() override;
};
END