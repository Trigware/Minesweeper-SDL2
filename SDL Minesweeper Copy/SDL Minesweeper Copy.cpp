#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include "Globals.h"
#include "Minesweeper Scene.h"
#include "Audio.h"
#include "TextRendering.h"
#include "Timer.h"

int main()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) { std::cerr << "SDL Initialization Error: " << SDL_GetError() << "\n"; return EXIT_FAILURE; }
	SDL_Window* window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.x, windowSize.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) { std::cerr << "SDL Window Creation Error: " << SDL_GetError() << "\n"; return EXIT_FAILURE; }
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) { std::cerr << "SDL Renderer Error: " << SDL_GetError() << "\n"; return EXIT_FAILURE; }
	int imageInitFlags = IMG_INIT_PNG;
	if ((IMG_Init(imageInitFlags) & imageInitFlags) == 0) { std::cerr << "SDL_image Initialization Error: " << SDL_GetError() << "\n"; return EXIT_FAILURE; }
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { std::cerr << "SDL_Mixer Initialization Error: " << SDL_GetError() << "\n"; return EXIT_FAILURE; }

	SDL_Event currentEvent;
	bool windowRunning = true;
	Spritesheet::Init();
	Audio::Init();
	Font::Init();

	MinesweeperScene currentScene;
	currentScene.OnStart();

	Audio::Play(AudioID::StartGame);
	uint32_t previousPerformCount = 0, currentPerformCount = 0;
	int frameCount = 0;

	while (windowRunning) {
		SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
		currentPerformCount = SDL_GetPerformanceCounter();
		double currentFreq = SDL_GetPerformanceFrequency();
		deltaTime = (currentPerformCount - previousPerformCount) / currentFreq;
		if (frameCount == 0) deltaTime = 0;
		for (int i = 0; i < Timer::allTimers.size(); i++) Timer::allTimers[i]->Progress();
		for (int i = 0; i < Animation::allAnimationManagers.size(); i++) Animation::allAnimationManagers[i]->Progress();

		while (SDL_PollEvent(&currentEvent)) {
			if (currentEvent.type == SDL_QUIT) windowRunning = false;
			if (currentEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
				windowSize = Vec2(currentEvent.window.data1, currentEvent.window.data2);
				currentScene.OnAspectChange();
			}
			if (currentEvent.type == SDL_MOUSEMOTION) currentScene.OnMouseMoved();
			bool mouseDown = currentEvent.type == SDL_MOUSEBUTTONDOWN, isLeft = currentEvent.button.button == SDL_BUTTON_LEFT, isRight = currentEvent.button.button == SDL_BUTTON_RIGHT, isPrimary = isLeft || isRight;
			if (mouseDown && isPrimary) currentScene.OnPrimaryMouseClick(isRight);
			bool escapeDown = currentEvent.type == SDL_KEYDOWN && currentEvent.key.repeat == 0 && currentEvent.key.keysym.sym == SDLK_ESCAPE;
			if (escapeDown) currentScene.OnEscapePressed();
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		currentScene.OnDraw();
		SDL_RenderPresent(renderer);
		previousPerformCount = currentPerformCount;
		frameCount++;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}