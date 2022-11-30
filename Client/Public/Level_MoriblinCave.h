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
	HRESULT Load_Objects_FromFile();
	HRESULT Load_Triggers_FromFile();
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag); 

	vector<CActor::MODELDESC> m_vInstancedObjects;

	vector<_float3> m_vDungeonCameras = 
	{
		_float3(0.f, 0.f, 0.f),		// 1A
		_float3(6.f, 0.f, 0.f),		// 1B
		_float3(12.f, 0.f, 0.f),	// 1C
		_float3(0.f, 0.f, -5.f),	// 2A
	};

public:
	static CLevel_MoriblinCave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END