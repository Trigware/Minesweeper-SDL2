#include "Minesweeper Scene.h"
#include <random>

void MinesweeperScene::InitializeAllMines() {
	int maximumMineCount = gridSize.x * gridSize.y - 9;
	minesCount = minesCount > maximumMineCount ? maximumMineCount : minesCount;
	minesPositions.clear();
	tileNeighbouringMinesCount.clear();
	uncoveredTiles.clear();
	explosionList.clear();
	uiAnimationTimer.Unpause();

	playTime.Unpause();
	labelUpdateTimer.functionOnTimeout();

	tilesWithoutMines.clear();
	for (int i = 0; i < directions.size(); i++) {
		Vec2<int> dir = directions[i], offsetedPos = firstClickedTile + dir;
		if (IsTileInvalid(offsetedPos)) continue;
		int offsetedIndex = IndexFromPos(offsetedPos);
		tilesWithoutMines.insert(offsetedIndex);
	}
	tilesWithoutMines.insert(IndexFromPos(firstClickedTile));

	explosionList.reserve(minesCount);
	for (int i = 0; i < minesCount; i++) {
		GenerateMine();
		explosionList.emplace_back(TextureID::Explosion, false);
	}
}

void MinesweeperScene::GenerateMine() {
	while (true) {
		Vec2<int> generatedPosition = GenerateTilePosition();
		int posIndex = IndexFromPos(generatedPosition);
		bool isSpawnInvalid = minesPositions.contains(posIndex) || tilesWithoutMines.contains(posIndex);
		if (isSpawnInvalid) continue;
		for (int i = 0; i < directions.size(); i++) UpdateMineNeighbourCount(generatedPosition, i);
		minesPositions.insert(posIndex);
		tilesList[posIndex].tilePosition = GetTileTexture(generatedPosition);
		return;
	}
}

Vec2<int> MinesweeperScene::GenerateTilePosition() {
	static std::random_device randomDevice;
	static std::mt19937 generator(randomDevice());
	std::uniform_int_distribution<int> xDistribution(0, gridSize.x - 1), yDistribution(0, gridSize.y - 1);
	Vec2<int> resultPosition = Vec2<int>(xDistribution(generator), xDistribution(generator));
	return resultPosition;
}

void MinesweeperScene::UpdateMineNeighbourCount(Vec2<int>& generatedPosition, int i) {
	Vec2<int>& dir = directions[i];
	Vec2<int> offsetedPos = generatedPosition + dir;
	if (IsTileInvalid(offsetedPos)) return;
	int offsetIndex = IndexFromPos(offsetedPos);
	if (minesPositions.contains(offsetIndex)) return;
	int numberOfMinesAtCurrent = tileNeighbouringMinesCount[offsetIndex] + 1;
	tileNeighbouringMinesCount[offsetIndex] = numberOfMinesAtCurrent;
	tilesList[offsetIndex].tilePosition = GetTileTexture(offsetedPos);
}