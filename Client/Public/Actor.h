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
public:
	typedef struct tagActorDescription
	{
		_tchar wcObjName[MAX_PATH];
		_tchar wcObjPrototypeId[MAX_PATH];
		_tchar wcModelPrototypeId[MAX_PATH];
		_tchar wcObjLayerTag[MAX_PATH];
		_float4x4 mObjPivotMatrix;
		//.. Class Tag?
	}ACTORDESC;

protected:
	CActor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor(const CActor& rhs);
	virtual ~CActor() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader*	m_pShaderCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;

protected:
	virtual HRESULT Ready_Components(void* pArg) = 0;
	virtual HRESULT SetUp_ShaderResources() = 0;

public:
	virtual void Free() override;
};
END