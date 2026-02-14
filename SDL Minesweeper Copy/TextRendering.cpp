#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "TextRendering.h"
#include "Sprite.h"
#include <variant>

FontID Label::defaultFont = FontID::LeafCrossFont;

void Label::ApplyFont(FontID fontID) {
	usedFont = Font::fontMap[fontID];
	usedFontID = fontID;
}

void Label::Draw() {
	for (int i = 0; i < glyphList.size(); i++) {
		Sprite& glyph = glyphList[i];
		glyph.Draw();
	}
}

AlignType::AlignType(float value) {
	int numberOfAlignmentTypes = static_cast<int>(Alignment::Custom);
	for (int i = 0; i < numberOfAlignmentTypes; i++) {
		Alignment currentAlignmentType = static_cast<Alignment>(i);
		float valueAtAlign = alignmentOptions.at(currentAlignmentType);
		if (value == valueAtAlign) { typeOfAlign = currentAlignmentType; return; }
	}
	typeOfAlign = Alignment::Custom;
}

Label::Label(LabelText text, AlignType align, Vec2<int> fontSize) : usedFontID(defaultFont), alignment(align), size(fontSize) {
	ApplyFont(defaultFont);
	Update(text);
}

Label::Label(AlignType align) : usedFontID(defaultFont), alignment(align) {
	ApplyFont(defaultFont);
}

void Label::Refresh(Vec2<int> pos, Vec2<float> fontSize) {
	position = pos;
	size = fontSize;
	Update(displayedText);
}

void Label::Update(LabelText text) {
	std::u16string wideText, previousText = displayedText;
	if (std::holds_alternative<std::u16string>(text)) wideText = std::get<std::u16string>(text);
	else {
		try { wideText = Font::UTF16(std::get<std::string>(text)); }
		catch (const std::range_error& e) { std::cerr << "When using wide text on labels, use std::u16string only!\n"; return; }
	}
	displayedText = wideText;
	if (previousText != displayedText) UpdateGlyphLineCount();

	glyphList.clear();
	int lineIndex = 0, curLineGlyphIndex = 0;
	for (int i = 0; i < wideText.size(); i++) {
		char16_t ch = wideText[i];
		if (ch == '\n') { curLineGlyphIndex = 0; lineIndex++; continue; }
		int index = usedFont.glyphPositions.at(ch);
		Sprite glyphSprite(usedFont.fontTextureID, CalculateGlyphPos(curLineGlyphIndex, lineIndex));
		glyphSprite.scale = size;
		glyphSprite.tilePosition.x = index;
		glyphSprite.modulate = modulate;
		glyphList.push_back(glyphSprite);
		curLineGlyphIndex++;
	}
}

Spritesheet Label::GetSpritesheet() { return Spritesheet(Font::fontToTexture[usedFontID]); }

void Label::UpdateGlyphLineCount() {
	glyphsAtLines.clear();
	int glyphsAtCurrentLine = 0, biggestLineGlyphCount = -1;
	std::u16string wideString = displayedText + u'\n';
	for (int i = 0; i < wideString.size(); i++) {
		char16_t ch = wideString[i];
		if (ch != '\n') { glyphsAtCurrentLine++; continue; }
		if (glyphsAtCurrentLine > biggestLineGlyphCount) {
			biggestLineGlyphCount = glyphsAtCurrentLine;
			longestLineIndex = glyphsAtLines.size();
		}
		glyphsAtLines.push_back(glyphsAtCurrentLine);
		glyphsAtCurrentLine = 0;
	}
}

int Label::GetLeftBounds() {
	UpdateGlyphLineCount();
	Vec2<int> firstGlyphPos = CalculateGlyphPos(0, longestLineIndex);
	return firstGlyphPos.x;
}

Vec2<int> Label::CalculateGlyphPos(int curLineGlyphIndex, int lineIndex) {
	Spritesheet spr = GetSpritesheet();
	int glyphCountAtCurrentLine = glyphsAtLines[lineIndex];
	int lineSize = glyphCountAtCurrentLine * spr.tileSize.x;
	float glyphSizeRatio = curLineGlyphIndex / (float)glyphCountAtCurrentLine, centeredRatio = glyphSizeRatio - alignment.alignmentValue;
	int posX = position.x + centeredRatio * lineSize * size.x,
		posY = position.y + lineIndex * spr.tileSize.y * size.y;
	return Vec2<int>(posX, posY);
}