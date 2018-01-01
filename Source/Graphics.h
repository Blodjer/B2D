#pragma once

#include <string>
#include <map>

class Graphics
{
public:
	Graphics();
	~Graphics();
	
	void Draw(class GameObject* gameObject);
	void Flip();
	void Clear();

	struct SDL_Surface* LoadSprite(const std::string& filePath);
	void BlitSurface(struct SDL_Texture* texture, struct SDL_Rect* source, struct SDL_Rect* target);

	struct SDL_Renderer* GetRenderer() const;

private:
	struct SDL_Window* Window;
	struct SDL_Renderer* Renderer;
	
	std::map<std::string, struct SDL_Surface*> LoadedSprites;
};

