#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Cube;
END

BEGIN(Client)
class CSky final : public CGameObject
{
public:
	enum SKYBOX
	{
		SKY_FIELD,
		SKY_MORIBLINCAVE,
		SKY_MARINHOUSE,
		SKY_END
	};

	typedef struct tagSkyDescription
	{
		SKYBOX m_eLevelId;
	} SKYDESC ;

private:
	CSky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_float fTimeDelta);
	virtual _uint Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CVIBuffer_Cube* m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT SetUp_ShaderResources();

private:
	SKYDESC m_tSkyDesc;

public:
	static CSky* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END