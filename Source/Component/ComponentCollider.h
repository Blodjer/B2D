#pragma once

#include "Component.h"

// sweep
// only rootcomponent sweeps
// setposition(SVector2 positon, bool sweep)

class CComponentCollider : public CComponent
{
public:
	CComponentCollider(CGameObject* pOwner);
	virtual ~CComponentCollider();

private:
	void OnContact() {}

};

