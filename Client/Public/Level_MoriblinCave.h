#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Actor.h"

BEGIN(Client)
class CLevel_MoriblinCave final : public CLevel
{
private:
	CLevel_MoriblinCave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MoriblinCave() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

public:
	HRESULT Load_From_File();
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag); 

	vector<CActor::MODELDESC> m_vInstancedObjects;

public:
	static CLevel_MoriblinCave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END