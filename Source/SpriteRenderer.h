#pragma once

#include "IComponentRenderer.h"

class SpriteRenderer : public IComponentRenderer
{
public:
	SpriteRenderer(GameObject* parent);
	virtual ~SpriteRenderer();

	void SetSprite(const std::string& filepath);

	virtual void Draw(Graphics* graphics);

	int Width;
	int Height;

private:
	SDL_Texture* Texture;
};
