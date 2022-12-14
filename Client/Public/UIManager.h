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
class CUIManager final : public CBase
{
	DECLARE_SINGLETON(CUIManager)

private:
	CUIManager();
	virtual ~CUIManager() = default;

public:
	HRESULT Initialize();
	_uint Tick(_float fTimeDelta);
	_uint Late_Tick(_float fTimeDelta);
	void Clear();

public:
	/* Initialize Functions */
	HRESULT Build_Hearts();
	HRESULT Build_ItemButtonX();
	HRESULT Build_ItemButtonY();

public:
	/* Manage Functions */
	void Get_PlayerHp();
	void Compute_Hearts();

private:
	/* Hearts Variables */
	vector<class CUI*> m_Hearts;
	_float m_fMaxHp = 0.f;
	_float m_fCurrentHp = 0.f;

public:
	virtual void Free() override;
};
END