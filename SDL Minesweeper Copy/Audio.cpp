#include "Audio.h"
#include <string>

std::unordered_map<AudioID, Audio> Audio::audioMap;

void Audio::Init() {
	int numberOfAudioFiles = static_cast<int>(AudioID::AudioCounter);
	std::string dir = "Audio/";
	for (int i = 0; i < numberOfAudioFiles; i++) {
		AudioID currentAudioID = static_cast<AudioID>(i);
		std::string relativePath = "";
		switch (currentAudioID) {
		case AudioID::Explode: relativePath = "Explode.wav"; break;
		case AudioID::AddFlag: relativePath = "AddFlag.wav"; break;
		case AudioID::RemoveFlag: relativePath = "RemoveFlag.wav"; break;
		case AudioID::StartGame: relativePath = "StartGame.wav"; break;
		case AudioID::UncoverArea: relativePath = "UncoverArea.wav"; break;
		case AudioID::UncoverTile: relativePath = "UncoverTile.wav"; break;
		case AudioID::WinGame: relativePath = "WinGame.wav"; break;
		}
		std::string fullPath = dir + relativePath;
		Mix_Chunk* currentChunk = Mix_LoadWAV(fullPath.c_str());
		Audio audioInstance(currentChunk);
		audioMap[currentAudioID] = audioInstance;
	}
}

void Audio::Play(AudioID audioID) {
	Mix_Chunk* currentChunk = audioMap[audioID].audioChunk;
	Mix_PlayChannel(-1, currentChunk, 0);
}