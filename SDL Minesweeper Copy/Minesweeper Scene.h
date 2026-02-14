#pragma once
#include "DrawManagement.h"
#include "Sprite.h"
#include "TextRendering.h"
#include "Timer.h"

struct GridTile {
	int tileNumber = 0;
	TileType tileType;
	GridTile(int num) : tileNumber(num), tileType(TileType::Number) {}
	GridTile(TileType type) : tileType(type) {}
};

enum class GameState {
	Playing,
	Lost,
	Won
};

class MinesweeperScene {
public:
	void OnStart();
	void OnAspectChange();
	void OnDraw();
	void OnMouseMoved();
	void OnPrimaryMouseClick(bool isRight);
	void OnEscapePressed();
	MinesweeperScene() = default;
private:
	const Color SelectedTile = Color(0x51D9DB);
	const float gridPadding = 0.825;
	const int amountOfNumberTiles = 8, maxPauseAlphaAfterGameEnds = 180;
	const float iconLabelOffset = 1.2, clockYMultiplier = 0.2, pauseMenuMultiplier = 0.75,
		pauseTextYPosMultiplier = 0.085, pauseTextSizeMultiplier = 0.85, mineExplosionAnimationDuration = 0.75, explosionSize = 1.5;
	const Vec2<float> buttonMenuSizeMultiplier = Vec2<float>(0.65, 0.15);
	int minesCount = 10;
	GameState currentState = GameState::Playing;
	Vec2<int> gridSize = Vec2(9, 9), firstClickedTile;
	std::unordered_set<int> minesPositions, uncoveredTiles, flaggedTiles, tilesWithoutMines, latestExploredTiles, lastIterationTiles;
	std::unordered_map<int, int> tileNeighbouringMinesCount;
	std::vector<Sprite> tilesList, explosionList;
	Label timerLabel = Label(Alignment::Center), flagCount = Label(Alignment::Center), pauseTextLabel = Label(GetPauseMenuText(), Alignment::Center);
	Sprite timerClockSprite = Sprite(TextureID::TimerClock), flagIconSprite = Sprite(TextureID::FlagIcon), pauseMain = Sprite(TextureID::PauseMain),
		pauseOutline = Sprite(TextureID::PauseOutline), newGameButton = Sprite(TextureID::Button, true, true);
	Timer playTime = Timer(true), labelUpdateTimer = Timer(1, true), uiAnimationTimer = Timer(0.4, true), mainMenuAnimationTimer = Timer(0.5, true);
	bool gameIsPaused = false, pausedBefore = false, canPause = true;

	int previousHoveredTile = 0;
	std::array<Vec2<int>, 8> directions = {
		Vec2<int>::X(-1), Vec2<int>::X(1), Vec2<int>::Y(-1), Vec2<int>::Y(1),
		Vec2<int>(-1, -1), Vec2<int>(1, -1), Vec2<int>(-1, 1), Vec2<int>(1, 1)
	};

	Vec2<int> GetOriginTilePosition(int& tileSize);
	void GenerateMine();
	Vec2<int> GenerateTilePosition();
	inline int IndexFromPos(Vec2<int> position);
	Vec2<int> PosFromIndex(int index);
	Vec2<int> GetTileTexture(Vec2<int> tilePosition);
	bool IsTileInvalid(Vec2<int> tilePosition);
	void UpdateMineNeighbourCount(Vec2<int>& generatedPosition, int i);
	void CreateTileSprite(Vec2<int> tilePosition, const Vec2<int>& originTilePosition, int tileSize);
	void InitializeAllMines();
	GridTile GetDisplayedTileInfo(Vec2<int> tilePosition);
	Vec2<int> GetTileOverMouse();
	void RightClickTile(Vec2<int> hoveredTile);
	void UncoverTile(Vec2<int> tilePosition);
	void FloodFillAutoUncover(Vec2<int> fromPos);
	void DisplayUI();
	void EndGame(bool hasLost);
	inline bool GameEnded() { return currentState != GameState::Playing; }
	const Color pauseMenuMainColor = Color(0x6A6766, 235), pauseMenuOutlineColor = Color(0xCDCDCD);
	void ResetHoveredColor() { tilesList[previousHoveredTile].modulate.Reset(); }
	void PauseGame();
	void UpdatePauseMenuDisplayInfo();
	void UpdateGameStateIconsAndLabels();
	void SetupTilesList();
	std::string GetPauseMenuText();
};