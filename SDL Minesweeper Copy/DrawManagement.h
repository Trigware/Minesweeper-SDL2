#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <concepts>
#include <unordered_set>
#include <array>
#include <variant>
#include <memory>
#include "Lerp.h"
#include "Timer.h"

template<typename T>
concept Vector = std::same_as<T, int> || std::same_as<T, float>;

template<Vector T>
struct Vec2 {
	T x = 0, y = 0;
	Vec2<T>(T paramX, T paramY) : x(paramX), y(paramY) {}
	Vec2<T>(T sameCoord) : x(sameCoord), y(sameCoord) {}
	Vec2() = default;

	static Vec2 X(T paramX) { return Vec2(paramX, 0); }
	static Vec2 Y(T paramY) { return Vec2(0, paramY); }
	T Get(bool gettingX) { return gettingX ? x : y; }

	Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
	Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
	Vec2 operator*(const Vec2& other) const { return Vec2(x * other.x, y * other.y); }
	Vec2 operator/(const Vec2& other) const { return Vec2(x / other.x, y / other.y); }
	bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
	bool operator!=(const Vec2& other) const { return !(*this == other); }

	friend std::ostream& operator<<(std::ostream& os, const Vec2& vec) {
		os << "(x: " << vec.x << ", y: " << vec.y << ")";
		return os;
	}

	template<Vector T2> explicit operator Vec2<T2>() const { return Vec2<T2>(static_cast<T2>(x), static_cast<T2>(y)); }
};

struct Color {
	uint8_t r, g, b, a;
	const static uint8_t Max = 0xFF;
	Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = Max) : r(red), g(green), b(blue), a(alpha) {}
	Color() : r(Max), g(Max), b(Max), a(Max) {}
	Color(uint32_t hex, uint8_t alpha = Max);
	void Reset();
};

enum class TextureID {
	TileSpritesheet,
	LeafCrossFont,
	TimerClock,
	FlagIcon,
	PauseMain,
	PauseOutline,
	Explosion,
	Button,
	TextureCounter
};

struct SheetData {
	std::string filePath;
	Vec2<int> tileSize;
	bool isSegmented;
	Vec2<int> upperLeftSize, bottomRightSize;
	SheetData(std::string path, const Vec2<int>& tileSizeVec) : filePath(path), tileSize(tileSizeVec), isSegmented(false) {}
	SheetData(std::string path, const Vec2<int>& upperLeft, const Vec2<int>& bottomRight) : filePath(path), isSegmented(true), upperLeftSize(upperLeft), bottomRightSize(bottomRight) {}
	SheetData() = default;
};

struct Spritesheet {
	static std::unordered_map<TextureID, Spritesheet> spritesheetMap;
	static void Init();
	Spritesheet() = default;
	Spritesheet(TextureID textureID);
	Spritesheet(SDL_Texture* texturePtr, Vec2<int> tileSizeVec, Vec2<int> imageSizeVec, bool segmented, const Vec2<int>& upperLeft, const Vec2<int>& bottomRight) :
		texture(texturePtr), tileSize(tileSizeVec), imageSize(imageSizeVec), tileCount(segmented ? 1 : imageSizeVec / tileSizeVec), isSegmented(segmented), upperLeftSize(upperLeft), bottomRightSize(bottomRight) {}
	SDL_Texture* texture;
	Vec2<int> tileSize, imageSize, tileCount;
	bool isSegmented = false;
	Vec2<int> upperLeftSize, bottomRightSize;
};