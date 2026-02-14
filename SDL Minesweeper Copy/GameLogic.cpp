#include "Minesweeper Scene.h"
#include "Audio.h"

void MinesweeperScene::FloodFillAutoUncover(Vec2<int> fromPos) {
	latestExploredTiles.clear();
	lastIterationTiles.clear();
	int initIndex = IndexFromPos(fromPos);
	lastIterationTiles.insert(initIndex);
	int numberOfIterations = 0;
	while (lastIterationTiles.size() > 0) {
		std::unordered_set<int> lastIterationSaved;
		for (int tileIndex : lastIterationTiles) {
			Vec2<int> tilePos = PosFromIndex(tileIndex);
			UncoverTile(tilePos);
			latestExploredTiles.insert(tileIndex);
			bool reachedNumberedTile = tileNeighbouringMinesCount.contains(tileIndex);
			if (reachedNumberedTile) continue;
			for (int i = 0; i < directions.size(); i++) {
				Vec2<int> dir = directions[i], offsetPos = tilePos + dir;
				int offsetIndex = IndexFromPos(offsetPos);
				bool alreadyExplored = latestExploredTiles.contains(offsetIndex), goingOutOfBounds = IsTileInvalid(offsetPos);
				if (alreadyExplored || goingOutOfBounds) continue;
				lastIterationSaved.insert(offsetIndex);
			}
		}
		lastIterationTiles = lastIterationSaved;
		numberOfIterations++;
	}
	labelUpdateTimer.functionOnTimeout();
	int targetInteractionCount = gridSize.x * gridSize.y - minesCount, interactionCount = uncoveredTiles.size();
	if (targetInteractionCount == interactionCount) EndGame(false);
	if (numberOfIterations > 1) { Audio::Play(AudioID::UncoverArea); return; }
	Audio::Play(AudioID::UncoverTile);
}

void MinesweeperScene::EndGame(bool hasLost) {
	Audio::Play(hasLost ? AudioID::Explode : AudioID::WinGame);
	currentState = hasLost ? GameState::Lost : GameState::Won;
	pauseTextLabel.Update(GetPauseMenuText());
	playTime.paused = true;
	if (hasLost) {
		for (Sprite& spr : explosionList) {
			spr.Show();
			spr.Animate(mineExplosionAnimationDuration, spr.GetTileCount().x);
		}
	}
	
	canPause = false;
	Timer::Wait(mineExplosionAnimationDuration, [=]() {
		PauseGame();
		canPause = true;
	});

	OnAspectChange();
}

void MinesweeperScene::UncoverTile(Vec2<int> tilePosition) {
	int tileIndex = IndexFromPos(tilePosition);
	uncoveredTiles.insert(tileIndex);
	if (flaggedTiles.contains(tileIndex)) flaggedTiles.erase(tileIndex);
	tilesList[tileIndex].tilePosition = GetTileTexture(tilePosition);
}

void MinesweeperScene::RightClickTile(Vec2<int> hoveredTile) {
	int hoveredIndex = IndexFromPos(hoveredTile);
	bool alreadyFlagged = flaggedTiles.contains(hoveredIndex), tileInteracted = uncoveredTiles.contains(hoveredIndex);
	if (tileInteracted) return;
	if (alreadyFlagged) flaggedTiles.erase(hoveredIndex);
	else flaggedTiles.insert(hoveredIndex);
	Audio::Play(alreadyFlagged ? AudioID::RemoveFlag : AudioID::AddFlag);
	tilesList[hoveredIndex].tilePosition = GetTileTexture(hoveredTile);
	labelUpdateTimer.functionOnTimeout();
}