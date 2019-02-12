#pragma once

#include "Component.h"
#include "Graphics.h"

class IComponentRenderer : public CComponent
{
public:
	using CComponent::CComponent;
	virtual void Draw(CGraphics* pGraphics) = 0;
};