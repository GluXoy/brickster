#include <SFML/Graphics.hpp>
#include "Structures.hpp"
#include <vector>
#include <iostream>

using namespace std;

void initHeart(GameEvents& gameEvents) {
    sf::Sprite heart;
    const sf::Vector2f size = {80.0f, 80.0f};
    heart.setTexture(gameEvents.heartTexture);
    heart.setScale(size.x / gameEvents.heartTexture.getSize().x, size.y / gameEvents.heartTexture.getSize().y);
    gameEvents.hearts.push_back(heart);
}

void initStartBlock(vector<Block>& building, GameEvents& gameEvents) {
    if (!gameEvents.startBlockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load startBlockTexture" << endl;
    }
    Block startBlock;
    const sf::Vector2f START_BLOCK_SIZE = { 190, 50 };
    startBlock.size = START_BLOCK_SIZE;
    startBlock.shape.setSize(startBlock.size); 

    const sf::Vector2f basePosition = { 
        (static_cast<float>(gameEvents.desktopMode.width) - startBlock.size.x) / 2, 
        static_cast<float>(gameEvents.desktopMode.height) - startBlock.size.y
    };

    startBlock.position = basePosition;
    startBlock.shape.setTexture(&gameEvents.startBlockTexture);
    startBlock.shape.setPosition(basePosition); 
    startBlock.shape.setFillColor(startBlock.buildingColor);
    
    building.push_back(startBlock);
    
    gameEvents.leftDropZone = building[0].position.x;
    gameEvents.rightDropZone = building[0].position.x + building[0].size.x;
}

void initLifesBox(GameEvents& gameEvents) {
    if (!gameEvents.heartTexture.loadFromFile("other/life.png")) {
        cerr << "Failed to load heart texture" << endl;
    }
    gameEvents.box.setSize({400, 100});
    gameEvents.box.setPosition({1520, 0});
    gameEvents.box.setFillColor(sf::Color::Transparent);

    float distanceBetweenHearts = 30.0f;
    sf::Vector2f startPosition(gameEvents.desktopMode.width - 110.0f, gameEvents.box.getPosition().y + 30.0f);

    for (int i = 0; i < gameEvents.lifes; i++) {
        initHeart(gameEvents);
        gameEvents.hearts[i].setPosition(startPosition);
        startPosition.x -= gameEvents.hearts[i].getGlobalBounds().width + distanceBetweenHearts;
    }
}

void initBrokenBlockTexture(GameEvents& gameEvents) {
    if (!gameEvents.brokenBlockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load brokenBlockTexture" << endl;
    }
}

void initBlockTexture(GameEvents& gameEvents) {
    if (!gameEvents.blockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load blockTexture" << endl;
    }
}

void initCrane(GameEvents& gameEvents) {
    if (!gameEvents.craneTexture.loadFromFile("other/crane.png")) {
        cerr << "Failed to load crane texture" << endl;
    }
    sf::Sprite crane;
    const sf::Vector2f size = {1920.0f, 1080.0f};
    crane.setTexture(gameEvents.craneTexture);
    crane.setScale(size.x / gameEvents.craneTexture.getSize().x, size.y / gameEvents.craneTexture.getSize().y);

    crane.setPosition({ 100, 0});
    gameEvents.crane = crane;
}

void initHook(GameEvents& gameEvents) {
    if (!gameEvents.hook.texture.loadFromFile("other/hook.png")) {
        cerr << "Failed to load hook texture" << endl;
    }

    gameEvents.hook.size = { 90.0f, 135.0f };
    gameEvents.hook.shape.setTexture(gameEvents.hook.texture);
    gameEvents.hook.shape.setScale(gameEvents.hook.size.x / gameEvents.hook.texture.getSize().x, gameEvents.hook.size.y / gameEvents.hook.texture.getSize().y);
    gameEvents.hook.shape.setColor(sf::Color(255, 255, 190));
}

void initFlashTexture(GameEvents& gameEvents) {
    if (!gameEvents.flashTexture.loadFromFile("other/flash.png")) {
        cerr << "Failed to load flash texture" << endl;
    }
    gameEvents.flash.setTexture(gameEvents.flashTexture);
}

void initScoreRectangle(GameEvents& gameEvents) {
    gameEvents.scoreRectangle.setSize(sf::Vector2f(250, 70));  
    gameEvents.scoreRectangle.setFillColor(sf::Color(0, 0, 0, 150)); 

    float xPosition = gameEvents.desktopMode.width - gameEvents.scoreRectangle.getSize().x - 10;
    float yPosition = gameEvents.desktopMode.height - gameEvents.scoreRectangle.getSize().y - 10;
    gameEvents.scoreRectangle.setPosition(xPosition, yPosition);
}

void initScoreText(GameEvents& gameEvents) {
    if (!gameEvents.font.loadFromFile("font/01.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }

    gameEvents.scoreText.setFont(gameEvents.font);
    gameEvents.scoreText.setCharacterSize(24);
    gameEvents.scoreText.setFillColor(sf::Color::White);
    gameEvents.scoreText.setString("Score: 10"); 

    gameEvents.scoreText.setPosition(gameEvents.scoreRectangle.getPosition().x + 10, gameEvents.scoreRectangle.getPosition().y + 10);
}

void initGame(std::vector<Block>& building, GameEvents& gameEvents) {
    initStartBlock(building, gameEvents);
    initLifesBox(gameEvents);
    initBrokenBlockTexture(gameEvents);
    initBlockTexture(gameEvents);
    initCrane(gameEvents);
    initHook(gameEvents);
    initFlashTexture(gameEvents);
    initScoreRectangle(gameEvents);
    initScoreText(gameEvents);
}