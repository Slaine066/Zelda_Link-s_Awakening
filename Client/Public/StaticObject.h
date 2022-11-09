#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)
class CStaticObject final : public CActor
{
public: 
	typedef struct tagStaticObjectDesc
	{

	}STATICOBJDESC;

private:
	CStaticObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStaticObject(const CStaticObject& rhs);
	virtual ~CStaticObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

public:
	static CStaticObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};
END