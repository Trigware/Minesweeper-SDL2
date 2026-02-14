#include "DrawManagement.h"
#include "Sprite.h"
#include <SDL_image.h>
#include "Globals.h"
#include "Timer.h"

std::unordered_map<TextureID, Spritesheet> Spritesheet::spritesheetMap = {};
std::vector<Animation*> Animation::allAnimationManagers = {};

void Spritesheet::Init() {
	int numberOfTextures = static_cast<int>(TextureID::TextureCounter);
	std::string dir = "Textures/", font = "Textures/Fonts/";
	for (int i = 0; i < numberOfTextures; i++) {
		TextureID currentTexture = static_cast<TextureID>(i);
		SheetData sheetData;
		switch (currentTexture) {
			case TextureID::TileSpritesheet: sheetData = SheetData(dir + "tileSpritesheet.png", Vec2(32, 32)); break;
			case TextureID::LeafCrossFont: sheetData = SheetData(font + "LeafCross Font.png", Vec2(18, 39)); break;
			case TextureID::TimerClock: sheetData = SheetData(dir + "Timer Clock.png", Vec2(22, 23)); break;
			case TextureID::FlagIcon: sheetData = SheetData(dir + "Flag Icon.png", Vec2(15, 23)); break;
			case TextureID::PauseMain: sheetData = SheetData(dir + "PauseMain.png", Vec2(300, 200)); break;
			case TextureID::PauseOutline: sheetData = SheetData(dir + "PauseOutline.png", Vec2(300, 200)); break;
			case TextureID::Explosion: sheetData = SheetData(dir + "ExplosionEffect.png", Vec2(64, 64)); break;
			case TextureID::Button: sheetData = SheetData(dir + "Button.png", Vec2(18, 18), Vec2(18, 18)); break;
		}
		SDL_Surface* imageSurface = IMG_Load(sheetData.filePath.c_str());
		Vec2<int> imageSize = Vec2(imageSurface->w, imageSurface->h);
		Vec2<int> tileSize = sheetData.isSegmented ? imageSize : sheetData.tileSize;
		spritesheetMap[currentTexture] = Spritesheet(SDL_CreateTextureFromSurface(renderer, imageSurface), tileSize, imageSize, sheetData.isSegmented, sheetData.upperLeftSize, sheetData.bottomRightSize);
	}
}

void Sprite::Draw() {
	if (!visible) return;
	if (tilePosition.x >= spritesheet.tileCount.x || tilePosition.y >= spritesheet.tileCount.y || tilePosition.x < 0 || tilePosition.y < 0) {
		return;
	}

	spriteSize = (Vec2<int>)((Vec2<float>)actualSize * scale);
	actualPosition = centered ? position - spriteSize / 2 : position;
	if (!spritesheet.isSegmented) {
		SDL_Rect sourceRect = { spritesheet.tileSize.x * tilePosition.x, spritesheet.tileSize.y * tilePosition.y, spritesheet.tileSize.x, spritesheet.tileSize.y },
			destinationRect = { actualPosition.x, actualPosition.y, actualSize.x * scale.x, actualSize.y * scale.y };
		DrawTexture(sourceRect, destinationRect);
		return;
	}

	DrawSegmentedSprite();
}

void Sprite::DrawSegmentedSprite() {
	upperLeft = spritesheet.upperLeftSize;
	bottomRight = spritesheet.bottomRightSize;
	Vec2<float> cornersSize = Vec2<float>(upperLeft.x + bottomRight.x, upperLeft.y + bottomRight.y), biggestCornersScale = (Vec2<float>)spriteSize / cornersSize;
	if (cornersScale.x > biggestCornersScale.x) cornersScale.x = biggestCornersScale.x;
	if (cornersScale.y > biggestCornersScale.y) cornersScale.y = biggestCornersScale.y;

	upperLeftDest = (Vec2<int>)((Vec2<float>)upperLeft * cornersScale);
	bottomRightDest = (Vec2<int>)((Vec2<float>)bottomRight * cornersScale);
	middleSourceSize = spritesheet.imageSize - upperLeft - bottomRight;
	middleDestinationSize = spriteSize - upperLeftDest - bottomRightDest;

	for (int y = 0; y < segmentPerCoord; y++) {
		for (int x = 0; x < segmentPerCoord; x++) {
			SDL_Rect sourceRect = { GetSourcePos(true, x), GetSourcePos(false, y), GetSourceSize(true, x), GetSourceSize(false, y) };
			SDL_Rect destinationRect = { GetDestinationPos(true, x), GetDestinationPos(false, y), GetDestinationSize(true, x), GetDestinationSize(false, y) };
			DrawTexture(sourceRect, destinationRect);
		}
	}
}

int Sprite::GetSegmentedPos(bool componentIsX, int coord, bool isSource) {
	Vec2<int> middleSize = isSource ? middleSourceSize : middleDestinationSize;
	Vec2<int> usedUpperLeft = isSource ? upperLeft : upperLeftDest;
	switch (coord) {
		case start: return 0;
		case middle: return usedUpperLeft.Get(componentIsX);
		case last: return usedUpperLeft.Get(componentIsX) + middleSize.Get(componentIsX);
	}
}

int Sprite::GetSegmentedSize(bool componentIsX, int coord, bool isSource) {
	Vec2<int> middleSize = isSource ? middleSourceSize : middleDestinationSize;
	Vec2<int> usedUpperLeft = isSource ? upperLeft : upperLeftDest, usedBottomRight = isSource ? bottomRight : bottomRightDest;
	switch (coord) {
		case start: return usedUpperLeft.Get(componentIsX);
		case middle: return middleSize.Get(componentIsX);
		case last: return usedBottomRight.Get(componentIsX);
	}
}

int Sprite::GetSourcePos(bool componentIsX, int coord) { return GetSegmentedPos(componentIsX, coord, true); }
int Sprite::GetDestinationPos(bool componentIsX, int coord) { return actualPosition.Get(componentIsX) + GetSegmentedPos(componentIsX, coord, false); }
int Sprite::GetSourceSize(bool componentIsX, int coord) { return GetSegmentedSize(componentIsX, coord, true); }
int Sprite::GetDestinationSize(bool componentIsX, int coord) { return GetSegmentedSize(componentIsX, coord, false); }

void Sprite::DrawTexture(const SDL_Rect& sourceRect, const SDL_Rect& destinationRect) {
	SDL_SetTextureColorMod(spritesheet.texture, modulate.r, modulate.g, modulate.b);
	SDL_SetTextureAlphaMod(spritesheet.texture, modulate.a);
	SDL_RenderCopy(renderer, spritesheet.texture, &sourceRect, &destinationRect);
}

Sprite::Sprite(TextureID texID, Vec2<int> posParam, Vec2<int> size) : spritesheet(Spritesheet(texID)), position(posParam) {
	actualSize = size == -1 ? spritesheet.tileSize : size;
}

Sprite::Sprite(TextureID texID, bool defaultVisibility, bool isCentered) {
	spritesheet = Spritesheet(texID);
	actualSize = spritesheet.tileSize;
	visible = defaultVisibility;
	centered = isCentered;
}

Spritesheet::Spritesheet(TextureID textureID) {
	*this = spritesheetMap[textureID];
}

Color::Color(uint32_t hex, uint8_t alpha) {
	a = alpha;
	r = (hex >> 16) & 0xFF;
	g = (hex >> 8) & 0xFF;
	b = hex & 0xFF;
}

void Color::Reset() {
	*this = Color(0xFFFFFF);
}

Spritesheet& Sprite::GetSpritesheet() {
	return spritesheet;
}

void Sprite::Animate(float duration, int endXFrame, LerpData data) {
	new Animation(*this, endXFrame, duration, data);
}

Animation::Animation(Sprite& spr, int endX, float duration, LerpData data) : linkedSprite(&spr), startXFrame(spr.tilePosition.x),
	lerpData(data), animationTimer(duration, false), endXFrame(endX) {
	allAnimationManagers.emplace_back(this);
}

void Animation::Progress() {
	float animationProgress = animationTimer.GetProgress();
	int tileX = Lerp::Get(startXFrame, endXFrame, animationProgress, lerpData);
	linkedSprite->tilePosition.x = tileX;
}