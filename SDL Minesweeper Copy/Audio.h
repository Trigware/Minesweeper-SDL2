#pragma once
#include "unordered_map"
#include <SDL_mixer.h>

enum class AudioID {
	Explode,
	AddFlag,
	RemoveFlag,
	StartGame,
	UncoverArea,
	UncoverTile,
	WinGame,
	AudioCounter
};

struct Audio {
	static std::unordered_map<AudioID, Audio> audioMap;
	static void Init();
	Mix_Chunk* audioChunk;
	Audio(Mix_Chunk* chunk) : audioChunk(chunk) {}
	Audio() = default;
	static void Play(AudioID audioID);
};