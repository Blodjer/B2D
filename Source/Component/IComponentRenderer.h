#pragma once

#include "Component/Component.h"
#include "Graphics/Renderer.h"

class CMaterial;

class IComponentRenderer : public CComponent
{
public:
	using CComponent::CComponent;

	virtual CMaterial const* GetMaterial() const = 0;
};