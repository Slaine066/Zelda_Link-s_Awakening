#pragma once

#include "Base.h"

BEGIN(Engine)
class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	char* Get_AnimationName() { return m_szName; }

public:
	HRESULT Initialize(class CModel* pModel, aiAnimation* pAIAnimation);

public:
	// "Invalidate_TransformationMatrix" this function transforms the Channels (Bones of the Animations) based on Time and KeyFrames.
	_bool Animate(_float fTimeDelta, _bool bIsLoop = true); 
	void Reset_Animation();

private:
	char m_szName[MAX_PATH] = "";

	_float m_fDuration = 0.f;
	_float m_fCurrentTime = 0.f;
	_float m_fTickPerSecond = 0.f;
	
	/* Bones needed to drive this Animation. */
	_uint m_iNumChannels = 0;
	vector<class CChannel*> m_Channels; /* Channel <-> Bone */

	_bool m_bIsFinished = false;

public:
	static CAnimation* Create(class CModel* pModel, aiAnimation* pAIAnimation);
	virtual void Free() override;
};
END