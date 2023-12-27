#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Block
{
    sf::RectangleShape shape;
    sf::Vector2f position;
    sf::Vector2f size;
};

struct BrokenBlockEvents {
    float brokenWidth = 0;
    Block brokenBlock;
    bool leftZone = false;
    bool rightZone = false;
};

struct Hook
{
    sf::Vector2f position = { 900, 130};
    sf::Vector2f size;
    sf::Texture texture;
    sf::Sprite shape;
};

struct CraneCable
{
    sf::RectangleShape shape;
    sf::Vector2f size;
    sf::Vector2f position;
    sf::Color color = sf::Color(128, 128, 128);
};

struct GameAssets {
    sf::Texture startBlockTexture;
    sf::Texture brokenBlockTexture;
    sf::Texture blockTexture;
    sf::Texture heartTexture;
    sf::Texture craneTexture;
    sf::Texture flashTexture;
    sf::Texture bombTexture;
    sf::Texture explosionTexture;
};

struct GameState {
    int blockCounter = 1;
    const float unbrokenPercent = 0.08;
    bool isNewBlock = true;
    bool isBlockFalling = false;
    bool showFlash = false;
    bool isShowScoreAnimation = false;
    bool isUpdateScoreAnimationPosFor10 = false;
    bool isUpdateScoreAnimationPosFor100 = false;
    bool isShowAntiBonusAnimation = false;
    bool isShowExplosion = false;
    int currentFontSize = 0;
    int targetFontSize = 450;
    float leftDropZone;
    float rightDropZone;
    CraneCable craneCable;
    BrokenBlockEvents brokenBlockEvents;
    int lifes = 3;
    int maxViewBlocks = 13;
};

struct AntiBonus {
    sf::Sprite shape;
    sf::Vector2f position;
    sf::Vector2f size = { 120, 130 };
    sf::Vector2f textureRectSize = { 300, 330 };
    float currentFrame;
    std::vector<int> list;
};

struct GameVisuals {
    sf::RectangleShape darknessRect;
    sf::RectangleShape scoreRectangle;
    sf::Font font;
    sf::Text scoreText;
    sf::Text animatedScoreText;
    sf::Sprite flash;
    sf::Sprite crane;
    sf::Sprite explosion;
    AntiBonus bomb;
    Hook hook;
    sf::RectangleShape box;
    std::vector<sf::Sprite> hearts;
};

struct GameTimers {
    sf::Clock flashTimer;
    sf::Clock scoreTimer;
    sf::Clock bombTimer;
    sf::Clock explosionTimer;
    float scoreAnimationDuration = 0.5;
    float bombAnimationDuration = 4.5;
    float bombAnimationDelay = 0.95;
    float explosionAnimationDuration = 0.5;
    float dTime;
};

struct BlockParams
{
    const sf::Vector2f startSize = { 190, 50 };
    sf::Vector2f startPosition;
    const float minWidth = 8.0f;
    sf::Vector2f position;
    sf::Color craneBlockColor = sf::Color(50, 240, 255);
    sf::Color buildingColor = sf::Color(50, 140, 255);
    float startCraneBlockSpeed = 500.0f;
    float craneBlockSpeed = startCraneBlockSpeed;
    float fallenBlockSpeed = 600.0f;
};

struct GameEvents {
    sf::VideoMode desktopMode;
    BlockParams blockParams;
    GameAssets assets;
    GameState state;
    GameVisuals visuals;
    GameTimers timers;
};
