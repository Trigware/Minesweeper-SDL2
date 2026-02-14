#include "Minesweeper Scene.h"
#include "Globals.h"
#include <iostream>

void MinesweeperScene::CreateTileSprite(Vec2<int> tilePosition, const Vec2<int>& originTilePosition, int tileSize) {
	Vec2<int> screenPosition = originTilePosition + tilePosition * tileSize;
	int tileIndex = IndexFromPos(tilePosition);
	Sprite& currentTile = tilesList[tileIndex];
	currentTile.position = screenPosition;
	currentTile.actualSize = tileSize;
	currentTile.tilePosition = GetTileTexture(tilePosition);
}

int MinesweeperScene::IndexFromPos(Vec2<int> position) {
	return position.y * gridSize.x + position.x;
}

Vec2<int> MinesweeperScene::PosFromIndex(int index) {
	return Vec2<int>(index % gridSize.x, index / gridSize.x);
}

bool MinesweeperScene::IsTileInvalid(Vec2<int> tilePosition) {
	return tilePosition.x < 0 || tilePosition.y < 0 || tilePosition.x >= gridSize.x || tilePosition.y >= gridSize.y;
}

std::string MinesweeperScene::GetPauseMenuText() {
	switch (currentState) {
		case GameState::Playing: return "Game Paused";
		case GameState::Won: return "You won!";
		case GameState::Lost: return "Game over!";
	}
}

Vec2<int> MinesweeperScene::GetTileOverMouse() {
	int tileSize; Vec2 originTilePosition = GetOriginTilePosition(tileSize);
	Vec2<int> originDiff = mousePosition - originTilePosition;
	Vec2<int> currentTile = originDiff / tileSize;
	if (originDiff.x < 0) currentTile.x--;
	if (originDiff.y < 0) currentTile.y--;
	return currentTile;
}

Vec2<int> MinesweeperScene::GetOriginTilePosition(int& tileSize) {
	Vec2<int> tileSizeInCoord = (Vec2<int>)((Vec2<float>)windowSize * gridPadding / (Vec2<float>)gridSize);
	tileSize = tileSizeInCoord.x < tileSizeInCoord.y ? tileSizeInCoord.x : tileSizeInCoord.y;
	Vec2<int> gridScreenSize = gridSize * tileSize;
	Vec2<int> originTilePosition = (windowSize - gridScreenSize) / 2;
	return originTilePosition;
}

Vec2<int> MinesweeperScene::GetTileTexture(Vec2<int> tilePosition) {
	GridTile gridTile = GetDisplayedTileInfo(tilePosition);
	int posX = amountOfNumberTiles + static_cast<int>(gridTile.tileType);
	if (gridTile.tileType == TileType::Number) posX = gridTile.tileNumber;
	bool isDark = tilePosition.x % 2 == tilePosition.y % 2;
	int posY = isDark ? 1 : 0;
	return Vec2<int>(posX, posY);
}

GridTile MinesweeperScene::GetDisplayedTileInfo(Vec2<int> tilePosition) {
	int tileIndex = IndexFromPos(tilePosition);
	bool isMineAtCurrentTile = minesPositions.contains(tileIndex), hasNotInteracted = !uncoveredTiles.contains(tileIndex), tileFlagged = flaggedTiles.contains(tileIndex);
	int numberOfMineNeighbours = 0;
	if (tileNeighbouringMinesCount.contains(tileIndex)) numberOfMineNeighbours = tileNeighbouringMinesCount[tileIndex];
	GridTile displayedTile(numberOfMineNeighbours);
	if (isMineAtCurrentTile) displayedTile.tileType = TileType::Mine;
	if (hasNotInteracted && currentState == GameState::Playing) displayedTile.tileType = TileType::NotInteracted;
	if (tileFlagged && (currentState == GameState::Playing || minesPositions.contains(tileIndex))) displayedTile.tileType = TileType::Flag;
	return displayedTile;
}

void MinesweeperScene::DisplayUI() {
	UpdateGameStateIconsAndLabels();
	UpdatePauseMenuDisplayInfo();
}

void MinesweeperScene::UpdateGameStateIconsAndLabels() {
	int tileSize; Vec2<int> originTilePosition = GetOriginTilePosition(tileSize);

	int labelYSize = timerLabel.GetSpritesheet().tileSize.y;
	int smallerGapSize = std::min(originTilePosition.x, originTilePosition.y);
	float labelSize = smallerGapSize / (float)labelYSize;
	Vec2<int> clockSpriteSize = timerClockSprite.GetSpritesheet().tileSize, flagIconSpriteSize = flagIconSprite.GetSpritesheet().tileSize;

	timerClockSprite.scale = flagIconSprite.scale = labelSize;
	int gridWidth = gridSize.x * tileSize;
	int xLabelPos = windowSize.x / 2, xTimerLabel = xLabelPos + clockSpriteSize.x * timerClockSprite.scale.x - gridWidth / 4;
	int xFlagIcon = xLabelPos + flagIconSpriteSize.x * flagIconSprite.scale.x + gridWidth / 4;
	int labelTrueSize = labelYSize * labelSize;
	int highestLabelY = originTilePosition.y - labelTrueSize;
	int lowestLabelY = highestLabelY - labelTrueSize;
	int yLabelPos = uiAnimationTimer.Lerp(lowestLabelY, highestLabelY, LerpData::InOutQuad);

	timerLabel.Refresh(Vec2<int>(xTimerLabel, yLabelPos), labelSize);
	timerLabel.Update(playTime.GetTime());
	flagCount.Refresh(Vec2<int>(xFlagIcon, yLabelPos), labelSize);
	int remainingFlags = minesCount - flaggedTiles.size();
	flagCount.Update(std::to_string(remainingFlags));

	int timerClockPosX = timerLabel.GetLeftBounds() - clockSpriteSize.x * labelSize * iconLabelOffset;
	int flagIconPoxX = flagCount.GetLeftBounds() - flagIconSpriteSize.x * labelSize * iconLabelOffset;
	int timerClockPosY = yLabelPos + labelYSize * timerClockSprite.scale.y * clockYMultiplier;
	timerClockSprite.position = Vec2<int>(timerClockPosX, timerClockPosY);
	flagIconSprite.position = Vec2<int>(flagIconPoxX, timerClockPosY);
	int uiStatsAlphaModulate = uiAnimationTimer.Lerp(0, Color::Max);
	timerLabel.modulate.a = flagCount.modulate.a = timerClockSprite.modulate.a = flagIconSprite.modulate.a = uiStatsAlphaModulate;
}

void MinesweeperScene::UpdatePauseMenuDisplayInfo() {
	Vec2<int> pauseMenuSize = pauseMain.GetSpritesheet().tileSize;
	float pauseMenuScaleFactor = std::min((float)windowSize.x / pauseMenuSize.x, (float)windowSize.y / pauseMenuSize.y) * pauseMenuMultiplier;
	Vec2<int> actualMenuSize = (Vec2<int>)((Vec2<float>)pauseMenuSize * pauseMenuScaleFactor);

	int pauseMenuX = windowSize.x / 2 - actualMenuSize.x / 2, finalMenuY = windowSize.y / 2 - actualMenuSize.y / 2;
	int startMenuY = finalMenuY - actualMenuSize.y;
	int pauseMenuY = mainMenuAnimationTimer.Lerp(startMenuY, finalMenuY, LerpData::InOutQuad);
	int alphaOutlineModulate = mainMenuAnimationTimer.Lerp(0, Color::Max, LerpData::InOutQuad);
	int usedMaxColor = currentState == GameState::Playing ? Color::Max : maxPauseAlphaAfterGameEnds;
	int alphaPauseMenuModulate = mainMenuAnimationTimer.Lerp(0, usedMaxColor, LerpData::InOutQuad);

	Vec2<int> pauseMenuPos(pauseMenuX, pauseMenuY);
	pauseMain.position = pauseOutline.position = pauseMenuPos;
	pauseMain.modulate = pauseMenuMainColor;
	pauseOutline.modulate = pauseMenuOutlineColor;
	pauseMain.actualSize = pauseOutline.actualSize = actualMenuSize;
	pauseMain.modulate.a = alphaPauseMenuModulate;
	pauseOutline.modulate.a = alphaOutlineModulate;
	Vec2<int> pauseLabelPosition(windowSize.x / 2, pauseMenuY + actualMenuSize.y * pauseTextYPosMultiplier / 2);
	pauseTextLabel.Refresh(pauseLabelPosition, pauseMenuScaleFactor * pauseTextSizeMultiplier);
	pauseTextLabel.modulate.a = alphaOutlineModulate;

	Vec2<int> newGameButtonPos = pauseMenuPos + actualMenuSize / 2;
	newGameButton.position = newGameButtonPos;
	newGameButton.actualSize = (Vec2<int>)((Vec2<float>)actualMenuSize * buttonMenuSizeMultiplier);
	std::cout << newGameButton.actualSize << std::endl;
}