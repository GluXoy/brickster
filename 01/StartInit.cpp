#include <SFML/Graphics.hpp>
#include "Structures.hpp"
#include <vector>
#include <iostream>
#include <ctime>

using namespace std;

void initHeart(GameEvents& gameEvents) {
    sf::Sprite heart;
    const sf::Vector2f size = {80.0f, 80.0f};
    heart.setTexture(gameEvents.assets.heartTexture);
    heart.setScale(size.x / gameEvents.assets.heartTexture.getSize().x, size.y / gameEvents.assets.heartTexture.getSize().y);
    gameEvents.visuals.hearts.push_back(heart);
}

void initStartBlock(vector<Block>& building, GameEvents& gameEvents) {
    if (!gameEvents.assets.startBlockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load startBlockTexture" << endl;
    }
    Block startBlock;
    startBlock.size = gameEvents.blockParams.startSize;
    startBlock.shape.setSize(startBlock.size); 

    const sf::Vector2f basePosition = { 
        (static_cast<float>(gameEvents.desktopMode.width) - startBlock.size.x) / 2 + 55, 
        static_cast<float>(gameEvents.desktopMode.height) - startBlock.size.y
    };

    gameEvents.blockParams.startPosition = basePosition;
    startBlock.position = basePosition;
    startBlock.shape.setTexture(&gameEvents.assets.startBlockTexture);
    startBlock.shape.setPosition(basePosition); 
    startBlock.shape.setFillColor(gameEvents.blockParams.buildingColor);
    
    building.push_back(startBlock);
    
    gameEvents.state.leftDropZone = building[0].position.x;
    gameEvents.state.rightDropZone = building[0].position.x + building[0].size.x;
}

void initLifesBox(GameEvents& gameEvents) {
    if (!gameEvents.assets.heartTexture.loadFromFile("other/life.png")) {
        cerr << "Failed to load heart texture" << endl;
    }
    gameEvents.visuals.box.setSize({400, 100});
    gameEvents.visuals.box.setPosition({1520, 0});
    gameEvents.visuals.box.setFillColor(sf::Color::Transparent);

    float distanceBetweenHearts = 30.0f;
    sf::Vector2f startPosition(gameEvents.desktopMode.width - 110.0f, gameEvents.visuals.box.getPosition().y + 30.0f);

    for (int i = 0; i < gameEvents.state.lifes; i++) {
        initHeart(gameEvents);
        gameEvents.visuals.hearts[i].setPosition(startPosition);
        startPosition.x -= gameEvents.visuals.hearts[i].getGlobalBounds().width + distanceBetweenHearts;
    }
}

void initBrokenBlockTexture(GameEvents& gameEvents) {
    if (!gameEvents.assets.brokenBlockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load brokenBlockTexture" << endl;
    }
}

void initBlockTexture(GameEvents& gameEvents) {
    if (!gameEvents.assets.blockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load blockTexture" << endl;
    }
}

void initCrane(GameEvents& gameEvents) {
    if (!gameEvents.assets.craneTexture.loadFromFile("other/crane.png")) {
        cerr << "Failed to load crane texture" << endl;
    }
    sf::Sprite crane;
    const sf::Vector2f size = {1920.0f, 1080.0f};
    crane.setTexture(gameEvents.assets.craneTexture);
    crane.setScale(size.x / gameEvents.assets.craneTexture.getSize().x, size.y / gameEvents.assets.craneTexture.getSize().y);

    crane.setPosition({ 100, 0});
    gameEvents.visuals.crane = crane;
}

void initHook(GameEvents& gameEvents) {
    if (!gameEvents.visuals.hook.texture.loadFromFile("other/hook.png")) {
        cerr << "Failed to load hook texture" << endl;
    }

    gameEvents.visuals.hook.size = { 90.0f, 135.0f };
    gameEvents.visuals.hook.shape.setTexture(gameEvents.visuals.hook.texture);
    gameEvents.visuals.hook.shape.setScale(gameEvents.visuals.hook.size.x / gameEvents.visuals.hook.texture.getSize().x, gameEvents.visuals.hook.size.y / gameEvents.visuals.hook.texture.getSize().y);
    gameEvents.visuals.hook.shape.setColor(sf::Color(255, 255, 190));
}

void initFlashTexture(GameEvents& gameEvents) {
    if (!gameEvents.assets.flashTexture.loadFromFile("other/flash.png")) {
        cerr << "Failed to load flash texture" << endl;
    }
    gameEvents.visuals.flash.setTexture(gameEvents.assets.flashTexture);
}

void initScoreRectangle(GameEvents& gameEvents) {
    gameEvents.visuals.scoreRectangle.setSize(sf::Vector2f(360, 110));  
    gameEvents.visuals.scoreRectangle.setFillColor(sf::Color(0, 0, 0, 150)); 

    float xPosition = gameEvents.desktopMode.width - gameEvents.visuals.scoreRectangle.getSize().x - 10;
    float yPosition = gameEvents.desktopMode.height - gameEvents.visuals.scoreRectangle.getSize().y - 10;
    gameEvents.visuals.scoreRectangle.setPosition(xPosition, yPosition);
}

void initScoreText(GameEvents& gameEvents) {
    if (!gameEvents.visuals.font.loadFromFile("font/01.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return;
    }

    gameEvents.visuals.scoreText.setFont(gameEvents.visuals.font);
    gameEvents.visuals.scoreText.setCharacterSize(40);
    gameEvents.visuals.scoreText.setFillColor(sf::Color::White);
    gameEvents.visuals.scoreText.setString("Score: 1"); 

    gameEvents.visuals.scoreText.setPosition(gameEvents.visuals.scoreRectangle.getPosition().x + 12, gameEvents.visuals.scoreRectangle.getPosition().y + 26);
}

void initAnimationScoreText(GameEvents& gameEvents) {
    gameEvents.visuals.animatedScoreText.setFont(gameEvents.visuals.font);
    gameEvents.visuals.animatedScoreText.setCharacterSize(gameEvents.state.currentFontSize);
    gameEvents.visuals.animatedScoreText.setFillColor(sf::Color(80, 80, 80));
    gameEvents.visuals.animatedScoreText.setString(to_string(gameEvents.state.blockCounter)); 

    gameEvents.visuals.animatedScoreText.setPosition(gameEvents.desktopMode.width / 2 - 190, gameEvents.desktopMode.height / 2 - 150);
}

void initAntiBonusTexture(GameEvents& gameEvents) {
    if (!gameEvents.assets.bombTexture.loadFromFile("other/bomb.png")) {
        cerr << "Failed to load bomb" << endl;
        return;
    }
    gameEvents.visuals.bomb.shape.setTexture(gameEvents.assets.bombTexture); 
    gameEvents.visuals.bomb.shape.rotate(70);
}

void initAntiBonusList(GameEvents& gameEvents) {
    srand(time(0));
    for(int i = 1; i < 101; i++) {
        int start = i * 10;
        int x = rand() % 10 + start;
        gameEvents.visuals.bomb.list.push_back(x);
    }
}

void initExplosionTexture(GameEvents& gameEvents) {
    if (!gameEvents.assets.explosionTexture.loadFromFile("other/explosion.png")) {
        cerr << "Failed to load explosion" << endl;
        return;
    }
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
    initAnimationScoreText(gameEvents);
    initAntiBonusTexture(gameEvents);
    initAntiBonusList(gameEvents);
    initExplosionTexture(gameEvents);
}