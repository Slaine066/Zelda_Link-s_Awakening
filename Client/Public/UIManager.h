#pragma once

#include "Client_Defines.h"
#include "Base.h"

/*
Note that this Class does NOT contain UI Objects.
(UI Objects are been stored in the ObjectManager, exactly like all the other Game Objects).

"Then why do we even need a UI Manager?"
This class just encloses a bunch of utility functions which allows us to work with UI Objects more comfortably.
*/

BEGIN(Client)
class CUIManager final : public CBase
{
	DECLARE_SINGLETON(CUIManager)

private:
	CUIManager();
	virtual ~CUIManager() = default;

public:
	HRESULT Initialize_UI();

public:
	virtual void Free() override;
};
END