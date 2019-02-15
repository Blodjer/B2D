#pragma once

#include "Component/Component.h"
#include "Graphics/Graphics.h"

class IComponentRenderer : public CComponent
{
public:
	using CComponent::CComponent;
	virtual void Draw(CGraphics* pGraphics) = 0;
};