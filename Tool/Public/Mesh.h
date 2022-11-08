#pragma once

#include "Tool_Defines.h"
#include "GameObject.h"
#include "Model.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Tool)
class CMesh final : public CGameObject
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

public:
	MODELDESC Get_ModelDesc() { return m_tModelDesc; }

	void Set_IsSelected(_bool bIsSelected) { m_bIsSelected = bIsSelected; }

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual _bool Picking(_float3& OutPos) override;

private:
	CShader*	m_pShaderCom = nullptr;
	CRenderer*	m_pRendererCom = nullptr;
	CTransform*	m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT SetUp_ShaderResources();

private:
	MODELDESC m_tModelDesc;

	_bool m_bIsSelected = false; // Selected from Tool List

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};
END