#pragma once
#include "DrawManagement.h"

class Sprite {
public:
	Vec2<int> position, tilePosition, actualSize;
	Vec2<float> scale = 1, cornersScale = 1;
	Color modulate;
	Sprite(TextureID texID, Vec2<int> posParam = 0, Vec2<int> size = -1);
	Sprite(TextureID texID, bool defaultVisibility, bool isCentered = false);
	Spritesheet& GetSpritesheet();
	void Draw();
	void Animate(float duration, int endXFrame, LerpData data = LerpData::Linear);
	Vec2<int> GetTileCount() { return spritesheet.tileCount; }
	void Show() { visible = true; }
	void Hide() { visible = false; }
	bool visible = true, centered = false;
private:
	Spritesheet spritesheet;
	void DrawTexture(const SDL_Rect& sourceRect, const SDL_Rect& destinationRect);
	void DrawSegmentedSprite();
	int GetSegmentedPos(bool componentIsX, int coord, bool isSource);
	int GetSegmentedSize(bool componentIsX, int coord, bool isSource);
	int GetSourcePos(bool componentIsX, int coord);
	int GetSourceSize(bool componentIsX, int coord);
	int GetDestinationPos(bool componentIsX, int coord);
	int GetDestinationSize(bool componentIsX, int coord);
	Vec2<int> middleSourceSize, middleDestinationSize, upperLeft, bottomRight, upperLeftDest, bottomRightDest, actualPosition, spriteSize;
	static const int segmentPerCoord = 3, start = 0, middle = 1, last = 2;
};

enum class TileType {
	Number,
	NotInteracted,
	Mine,
	Flag
};

class Animation {
public:
	static std::vector<Animation*> allAnimationManagers;
	Animation(Sprite& spr, int endX, float duration, LerpData data);
	void Progress();
private:
	Sprite* linkedSprite = nullptr;
	int startXFrame, endXFrame;
	Timer animationTimer;
	LerpData lerpData = LerpData::Linear;
};
