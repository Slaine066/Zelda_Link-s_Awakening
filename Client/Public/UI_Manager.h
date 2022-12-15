#pragma once

#include "Client_Defines.h"
#include "Base.h"

/*
Note that this Class contains a pointer of the same Object stored in the Object Manager.
"Then why do you need to store them twice?"
This class just encloses a bunch of utility functions which allows us to work with UI Objects more comfortably.
(By doing so, we don't need to serach in the Object Manager every time).
*/

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	enum MODE { MODE_GAME, MODE_MAP, MODE_INVENTORY, MODE_END };

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();
	_uint Tick(_float fTimeDelta);
	void Handle_Input();
	void Clear();

private:
	/* Initialize Functions */
	HRESULT Build_Hearts();
	HRESULT Build_ItemButtonX();
	HRESULT Build_ItemButtonY();

public:
	MODE Get_Mode() { return m_eMode; }
	void Set_Mode(MODE eMode) { m_eMode = eMode; }
	_float Get_MaxHp() { return m_fMaxHp; }
	_float Get_CurrentHp() { return m_fCurrentHp; }

	/* Manage Functions */
	void Get_PlayerHp();
	void Compute_Hearts();

private:
	MODE m_eMode = MODE_END;

	/* Hearts Variables */
	vector<class CUI_Heart*> m_Hearts;
	_float m_fMaxHp = 0.f;
	_float m_fCurrentHp = 0.f;

public:
	virtual void Free() override;
};
END