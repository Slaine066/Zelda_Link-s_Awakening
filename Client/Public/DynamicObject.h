#pragma once

#include "Client_Defines.h"
#include "Actor.h"

/* Class for Dynamic Objects, that is:
- Object with Animations;
- Object with specific Logic.

A good example would be a:
- Dungeon Door (which opens after clearing the room). */

BEGIN(Client)
class CDynamicObject abstract : public CActor
{
protected:
	CDynamicObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDynamicObject(const CDynamicObject& rhs);
	virtual ~CDynamicObject() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_float fTimeDelta) override;
	virtual _uint Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override;

	_uint Compute_LevelIndex(_tchar* pObjName);

public:
	virtual void Free() override;
};
END