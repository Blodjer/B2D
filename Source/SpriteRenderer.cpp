#include "SpriteRenderer.h"
#include <string>
#include <map>
#include <SDL2\SDL_image.h>
#include <SDL2\SDL.h>
#include "GameEngine.h"
#include "Graphics.h"
#include "GameObject.h"
#include "Debug.h"

SpriteRenderer::SpriteRenderer(GameObject* pOwner) : IComponentRenderer(pOwner)
{
	this->SetSprite("Content/Sprites/MissingTexture.png");
}
static std::map<SDL_Surface*, SDL_Texture*> LoadedTextures;

SpriteRenderer::~SpriteRenderer()
{
	for (auto a : LoadedTextures)
	{
		SDL_DestroyTexture(a.second);
	}
	LoadedTextures.clear();
}

void SpriteRenderer::SetSprite(const std::string& filePath)
{
	auto graphics = GameEngine::Instance()->GetGraphicsInstance();
	auto surface = graphics->LoadSprite(filePath);
	if (surface == NULL)
		surface = graphics->LoadSprite("Content/Sprites/MissingTexture.png");

	this->Width = surface->w;
	this->Height = surface->h;

	if (LoadedTextures.find(surface) == LoadedTextures.end())
	{
		LoadedTextures[surface] = SDL_CreateTextureFromSurface(graphics->GetRenderer(), surface);
	}

	this->Texture = LoadedTextures[surface];
}

void SpriteRenderer::Draw(Graphics* graphics)
{
	SDL_Rect a;
	a.x = 0;
	a.y = 0;
	a.w = this->Width;
	a.h = this->Height;

	SDL_Rect b;
	b.x = -this->Width * 0.5f + this->GetWorldPosition().X;
	b.y = -this->Height * 0.5f + this->GetWorldPosition().Y;
	b.w = this->Width;
	b.h = this->Height;
	
	graphics->BlitSurface(Texture, &a, &b);
}
