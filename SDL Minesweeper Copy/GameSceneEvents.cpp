#include "Minesweeper Scene.h"
#include "Globals.h"

void MinesweeperScene::OnDraw() {
	for (Sprite& spr : tilesList) spr.Draw();
	for (Sprite& spr : explosionList) spr.Draw();
	timerClockSprite.Draw();
	timerLabel.Draw();
	flagCount.Draw();
	flagIconSprite.Draw();
	if (!uiAnimationTimer.Timeout()) DisplayUI();
	pauseMain.Draw();
	pauseOutline.Draw();
	pauseTextLabel.Draw();
	newGameButton.Draw();
}

void MinesweeperScene::OnAspectChange() {
	DisplayUI();
	int tileSize; Vec2<int> originTilePosition = GetOriginTilePosition(tileSize);
	for (int y = 0; y < gridSize.y; y++) {
		for (int x = 0; x < gridSize.y; x++) {
			CreateTileSprite(Vec2<int>(x, y), originTilePosition, tileSize);
		}
	}

	int mineIndex = 0;
	for (int minePosInt : minesPositions) {
		Vec2<int> mineGridPos = PosFromIndex(minePosInt), unoffsetedPos = originTilePosition + mineGridPos * tileSize;
		Vec2<int> spriteOffset = tileSize * (explosionSize - 1) / 2, minePosition = unoffsetedPos - spriteOffset;
		Sprite& explosionSpr = explosionList[mineIndex];
		explosionSpr.position = minePosition;
		explosionSpr.actualSize = tileSize * explosionSize;
		mineIndex++;
	}
}

void MinesweeperScene::OnEscapePressed() {
	if (!canPause) return;
	PauseGame();
}

void MinesweeperScene::PauseGame() {
	gameIsPaused = !gameIsPaused;
	if (pausedBefore) mainMenuAnimationTimer.Reverse();
	else mainMenuAnimationTimer.Unpause();
	if (gameIsPaused) ResetHoveredColor();
	if (uncoveredTiles.size() > 0 && currentState == GameState::Playing) playTime.paused = gameIsPaused;
	pausedBefore = true;
}

void MinesweeperScene::OnStart() {
	labelUpdateTimer.functionOnTimeout = [=]() {
		DisplayUI();
		labelUpdateTimer.Restart();
	};

	SetupTilesList();
	OnAspectChange();
	OnMouseMoved();
}

void MinesweeperScene::SetupTilesList() {
	const int tileCount = gridSize.x * gridSize.y;
	tilesList.reserve(tileCount);
	for (int i = 0; i < tileCount; i++) tilesList.emplace_back(TextureID::TileSpritesheet);
}

void MinesweeperScene::OnPrimaryMouseClick(bool isRight) {
	if (GameEnded() || gameIsPaused) return;
	Vec2<int> hoveredTile = GetTileOverMouse();
	if (IsTileInvalid(hoveredTile)) return;
	if (isRight) { RightClickTile(hoveredTile); return; }
	int hoveredIndex = IndexFromPos(hoveredTile);

	if (uncoveredTiles.size() == 0) {
		firstClickedTile = hoveredTile;
		InitializeAllMines();
	}

	bool clickedOnMine = minesPositions.contains(hoveredIndex), clickedOnUncovered = uncoveredTiles.contains(hoveredIndex);
	if (flaggedTiles.contains(hoveredIndex)) flaggedTiles.erase(hoveredIndex);
	if (clickedOnMine) { EndGame(true); return; }

	if (!clickedOnUncovered) FloodFillAutoUncover(hoveredTile);
	tilesList[hoveredIndex].modulate.Reset();
}

void MinesweeperScene::OnMouseMoved() {
	Vec2<int> currentTile = GetTileOverMouse();
	int spritesIndex = IndexFromPos(currentTile);
	bool hoveringAboveInvalidTile = IsTileInvalid(currentTile), hasInteractedWithTile = uncoveredTiles.contains(spritesIndex);
	ResetHoveredColor();
	if (hoveringAboveInvalidTile || hasInteractedWithTile || GameEnded() || gameIsPaused) return;
	tilesList[spritesIndex].modulate = SelectedTile;
	previousHoveredTile = spritesIndex;
}