#pragma once

#include "IComponent.h"
#include "Graphics.h"

class IComponentRenderer : public IComponent
{
public:
	using IComponent::IComponent;
	virtual void Draw(Graphics* pGraphics) = 0;
};