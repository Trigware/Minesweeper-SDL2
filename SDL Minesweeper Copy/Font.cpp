#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include "TextRendering.h"
#include <iostream>
#include <fstream>

std::unordered_map<FontID, Font> Font::fontMap;
std::unordered_map<FontID, TextureID> Font::fontToTexture = {
	{FontID::LeafCrossFont, TextureID::LeafCrossFont}
};

void Font::Init() {
	int fontCounter = static_cast<int>(FontID::FontCounter);
	for (int i = 0; i < fontCounter; i++) {
		FontID currentFontID = static_cast<FontID>(i);
		std::string path = "";
		switch (currentFontID) {
		case FontID::LeafCrossFont: path = "leafcross"; break;
		}
		std::filesystem::path absolutePath = std::filesystem::current_path() / "FontData" / (path + ".fnt");
		Font currentFont = ReadFromFile(absolutePath, currentFontID);
		fontMap[currentFontID] = currentFont;
	}
}

std::u16string Font::UTF16(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
	return converter.from_bytes(str);
}

Font Font::ReadFromFile(std::filesystem::path absolutePath, FontID fontID) {
	std::ifstream file(absolutePath);
	std::string currentLine, fileContents;
	Font currentFont;
	while (std::getline(file, currentLine)) fileContents += currentLine;
	file.close();

	currentFont.fontTextureID = fontToTexture[fontID];
	std::u16string fontData = UTF16(fileContents);

	for (int i = 0; i < fontData.size(); i++) {
		char16_t ch = fontData[i];
		currentFont.glyphPositions[ch] = i;
	}
	return currentFont;
}