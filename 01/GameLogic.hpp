#pragma once

#include <SFML/Graphics.hpp>
#include "Structures.hpp"
#include <vector>

Block initBlock(Block& prevBlock, GameEvents& gameEvents);
void initCraneCable(CraneCable& craneCable, sf::Vector2f position);
void initAntiBonus(GameEvents& gameEvents);
void updateBlockAndHookOnCrane(Block& block, GameEvents& gameEvents);
void initBrokenBlock(GameEvents& gameEvents, const float buildingHeight, float blockHeight);
void initSuccesfulFlash(GameEvents& gameEvents, sf::Vector2f blockSize, sf::Vector2f position, std::vector<Block>& building);
void collisionBlock(Block& block, GameEvents& gameEvents, const float buildingHeight, std::vector<Block>& building);
void updateFallingBlock(Block& block, std::vector<Block>& building, GameEvents& gameEvents);
void pollEvents(sf::RenderWindow& window, GameEvents& gameEvents, Block& block);
void redrawFrame(sf::RenderWindow& window, Block& block, std::vector<Block>& building, sf::Sprite& backgroundSprite, GameEvents& gameEvents);
void updateBuildingPositions(GameEvents& gameEvents, std::vector<Block>& building, float newBuildingHeight);
void updateBuildingHeight(std::vector<Block>& building, GameEvents& gameEvents);
void updateBrokenBlock(GameEvents& gameEvents);
void updateScoreAnimation(GameEvents& gameEvents);
void changeBlockCraneSpeed(GameEvents& gameEvents);
void updateAntiBonusAnimation(GameEvents& gameEvents);
void collisionAntiBonus(GameEvents& gameEvents);
void updateBuildingAfterAntiBonus(GameEvents& gameEvents, std::vector<Block>& building);
void gameProcess(GameEvents& gameEvents, std::vector<Block>& building, Block& block);

