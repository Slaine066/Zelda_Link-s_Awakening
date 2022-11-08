#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)
class CActor abstract : public CGameObject
{
public:
	/*
	*	Client Info
	*	Change Structure below in order to be able to use this Tool with your Client specific Data.
	*/

	// Struct used to Load and Save Data to Files
	typedef struct tagModelDescription
	{
		// Model Infos
		_tchar wcFileName[MAX_PATH];			// "Fiona.fbx"
		_tchar wcFilePath[MAX_PATH];			// "../../Resources/Meshes/Anim/Fiona
		_tchar wcModelPrototypeId[MAX_PATH];	// "Prototype_Component_Model_Fiona"
		CModel::TYPE eModelType;				// TYPE::TYPE_NONANIM or TYPE::TYPE_ANIM

												// Object Infos
		_tchar wcObjName[MAX_PATH];				// "Fiona"
		_tchar wcObjPrototypeId[MAX_PATH];		// "Prototype_GameObject_Player"

		// Layer Infos
		_tchar wcLayerTag[MAX_PATH];			// "Layer_Player"

		// Transform Infos
		_float4x4 mWorldMatrix;
	}MODELDESC;

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
	virtual CCollider* Get_Collider(CCollider::AIM eAim) override;

	void Render_Colliders();

protected:
	CShader*	m_pShaderCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;
	vector<CCollider*> m_vCollidersCom;

protected:
	MODELDESC m_tModelDesc;

protected:
	virtual HRESULT Ready_Components(void* pArg) = 0;
	virtual HRESULT SetUp_ShaderResources() = 0;

public:
	virtual void Free() override;
};
END