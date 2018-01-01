#include "Graphics.h"
#include <SDL2\SDL.h>
#include <SDL2\SDL_image.h>
#include "GameObject.h"
#include "IComponentRenderer.h"
#include "ComponentCollider.h"
#include "Debug.h"

Graphics::Graphics()
{
	SDL_CreateWindowAndRenderer(1024, 720, 0, &this->Window, &this->Renderer);
	SDL_SetWindowTitle(this->Window, "HelloGameTitle");
	SDL_SetRenderDrawBlendMode(this->Renderer, SDL_BLENDMODE_BLEND);
}

Graphics::~Graphics()
{
	SDL_DestroyWindow(this->Window);
	SDL_DestroyRenderer(this->Renderer);

	for (auto pSprite : this->LoadedSprites)
	{
		SDL_FreeSurface(pSprite.second);
	}
	this->LoadedSprites.clear();
}

void Graphics::Draw(GameObject* pGameObject)
{
	for (auto pComponent : pGameObject->GetComponents())
	{
		auto pComponentRenderer = dynamic_cast<IComponentRenderer*>(pComponent);
		if (pComponentRenderer != NULL)
			pComponentRenderer->Draw(this);
	}

	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(this->Renderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(this->Renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
	for (auto pComponent : pGameObject->GetComponents())
	{
		auto pComponentCollider = dynamic_cast<ComponentCollider*>(pComponent);
		if (pComponentCollider != NULL)
			pComponentCollider->DrawShape(this->Renderer);
	}
	SDL_SetRenderDrawColor(this->Renderer, r, g, b, a);
}

void Graphics::Flip()
{
	SDL_RenderPresent(this->Renderer);
}

void Graphics::Clear()
{
	SDL_RenderClear(this->Renderer);
}

SDL_Surface* Graphics::LoadSprite(const std::string& filePath)
{
	if (this->LoadedSprites.find(filePath) == LoadedSprites.end())
	{
		this->LoadedSprites[filePath] = IMG_Load(filePath.c_str());
	}

	return this->LoadedSprites[filePath];
}

void Graphics::BlitSurface(SDL_Texture* texture, SDL_Rect* source, SDL_Rect* target)
{
	SDL_RenderCopy(this->Renderer, texture, source, target);
}

SDL_Renderer* Graphics::GetRenderer() const
{
	return this->Renderer;
}
