#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Actor.h"

BEGIN(Client)
class CLevel_MoriblinCave final : public CLevel
{
public:
	typedef struct tagDungeonRoomDescription
	{
		_float4 m_vRoomPosition;
		vector<class CMonster*> m_RoomMonsters;
		_bool m_bIsRoomClear = false;
	} DUNGEONROOMDESC;

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
	HRESULT Ready_Sky();
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag); 

	DUNGEONROOMDESC* Get_CurrentRoom() { return m_pCurrentDungeonRoom; }
	_bool Get_RoomChanged() { return m_bIsRoomChanged; }

	void Add_RoomPositions();
	void Compute_RoomsData(); /* Used to compute which Monster is in which Room. */
	void Compute_CurrentRoom();
	void Remove_MonsterFromRoom(class CMonster* pMonster);
	void Check_Doors(_float fTimeDelta);
	void Close_Door();

private:
	vector<CActor::MODELDESC> m_vInstancedObjects;
	class CUI_Manager* m_pUIManager = nullptr;
	
	vector<DUNGEONROOMDESC*> m_DungeonRoomDescs;
	DUNGEONROOMDESC* m_pCurrentDungeonRoom = nullptr;
	_bool m_bIsRoomChanged = false;

	vector<class CDungeonDoor*> m_DungeonDoors;
	_bool m_bShouldClose = false;
	_float m_fCloseTimer = 0.f;
	_float m_fCloseTimerMax = .5f;

public:
	static CLevel_MoriblinCave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END