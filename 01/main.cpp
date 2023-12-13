#include "StartInit.hpp"
#include "GameLogic.hpp"
#include "Structures.hpp"

using namespace std;

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode desktopMode = sf::VideoMode(1920, 1080);
    sf::RenderWindow window(
        desktopMode,
        "BRICKSTER",
        sf::Style::None,
        settings
    );

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background/07.jpg")) {
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(window.getSize().x / backgroundSprite.getLocalBounds().width, 
                            window.getSize().y / backgroundSprite.getLocalBounds().height);

    sf::Clock clock;
    GameEvents gameEvents;
    gameEvents.desktopMode = desktopMode;

    const float DARKNESS_DEGREE = 0.75;
    sf::RectangleShape darknessRect(sf::Vector2f(window.getSize().x, window.getSize().y));
    darknessRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(DARKNESS_DEGREE * 255)));
    gameEvents.darknessRect = darknessRect;

    vector<Block> building;

    initGame(building, gameEvents);
    
    Block newBlock;

    while (window.isOpen() && gameEvents.lifes) {
        float dTime = clock.restart().asSeconds();
        gameEvents.dTime = dTime;

        if (gameEvents.isNewBlock) {
            newBlock = initBlock(building[building.size() - 1], gameEvents);
            gameEvents.isNewBlock = false;
        }

        pollEvents(window, gameEvents, newBlock);
        gameProcess(gameEvents, building, newBlock);
        redrawFrame(window, newBlock, building, backgroundSprite, gameEvents);
    }

    return 0;
}