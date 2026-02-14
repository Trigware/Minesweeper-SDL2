#pragma once
#include <string>
#include <unordered_map>
#include "DrawManagement.h"
#include <filesystem>
#include <vector>
#include <codecvt>
#include <variant>
#include "Sprite.h"

enum class FontID {
	LeafCrossFont,
	FontCounter
};

struct Font {
	static std::unordered_map<FontID, Font> fontMap;
	static std::unordered_map<FontID, TextureID> fontToTexture;
	static void Init();
	static Font ReadFromFile(std::filesystem::path path, FontID fontID);
	std::unordered_map<char16_t, int> glyphPositions;
	TextureID fontTextureID;
	static std::u16string UTF16(const std::string& str);
};

using LabelText = std::variant<std::string, std::u16string>;

enum class Alignment {
	Left,
	Center,
	Right,
	Custom
};

struct AlignHash { int operator()(Alignment align) const { return static_cast<int>(align); } };

struct AlignType {
	const inline static std::unordered_map<Alignment, float, AlignHash> alignmentOptions = { {Alignment::Left, 0}, {Alignment::Center, 0.5}, {Alignment::Right, 1} };
	float alignmentValue = 0;
	Alignment typeOfAlign = Alignment::Left;
	AlignType(float value);
	AlignType(Alignment type) : typeOfAlign(type), alignmentValue(alignmentOptions.at(type)) {}
};

class Label {
public:
	Vec2<int> position;
	Vec2<float> size;
	Color modulate;
	Label(LabelText text, AlignType align = AlignType(Alignment::Left), Vec2<int> fontSize = 1);
	Label(AlignType align);
	Label() {};
	void ApplyFont(FontID fontID);
	void Draw();
	void Update(LabelText text);
	void Refresh(Vec2<int> pos, Vec2<float> fontSize = 1);
	void UpdateGlyphLineCount();
	int GetLeftBounds();
	Spritesheet GetSpritesheet();
	static FontID defaultFont;
	AlignType alignment = Alignment::Left;
	std::u16string displayedText;
private:
	Font usedFont; FontID usedFontID;
	std::vector<Sprite> glyphList;
	std::vector<int> glyphsAtLines;
	int longestLineIndex = -1;
	Vec2<int> CalculateGlyphPos(int curLineGlyphIndex, int lineIndex);
};