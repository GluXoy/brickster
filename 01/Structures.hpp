#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

struct Block
{
    sf::RectangleShape shape;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Color craneBlockColor = sf::Color(50, 240, 255);
    sf::Color buildingColor = sf::Color(50, 140, 255);
    float craneBlockSpeed = 500.0f;
    float fallenBlockSpeed = 700.0f;
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

struct GameEvents
{
    int blockCounter = 1;
    const float unbrokenPercent = 0.085;
    sf::VideoMode desktopMode;
    sf::RectangleShape darknessRect;
    sf::RectangleShape scoreRectangle;
    sf::Font font;
    sf::Text scoreText;
    bool isNewBlock = true;
    bool isBlockFalling = false;
    float dTime;
    float leftDropZone;
    float rightDropZone;
    CraneCable craneCable;
    sf::Texture startBlockTexture;
    sf::Texture brokenBlockTexture;
    sf::Texture blockTexture;
    sf::Texture heartTexture;
    sf::Texture craneTexture;
    sf::Texture flashTexture;
    sf::Sprite flash;
    sf::Sprite crane;
    Hook hook;
    BrokenBlockEvents brokenBlockEvents;
    int lifes = 3;
    int maxViewBlocks = 13;
    sf::RectangleShape box;
    std::vector<sf::Sprite> hearts;
    //
    sf::Clock flashTimer;
    bool showFlash = false;
}; 