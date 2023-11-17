#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct Block
{
    sf::RectangleShape shape;
    sf::Vector2f position; //заменить позже на константу начальных координат появляющегося блока на кране
    sf::Vector2f size = { 200, 100 };

    float craneBlockSpeed = 1000.0f;
    float fallenBlockSpeed = 600.0f;
};

struct GameEvents
{
    bool isNewBlock = true;
    bool isBlockFalling = false;
    float dTime;
};

Block initBlock() {
    Block block;
    block.shape.setSize(block.size); 
    block.shape.setPosition({0, 0}); 
    block.shape.setFillColor(sf::Color::Red);
    return block;
}

void updateBlockOnCrane(sf::VideoMode desktopMode, Block& block, GameEvents& gameEvents) {
    if (!gameEvents.isBlockFalling) { 
        float dTime = gameEvents.dTime;
        const float speed = block.craneBlockSpeed;
        const float offset = speed * dTime;

        const float screenWidth = desktopMode.width;
        const float blockPositionX = block.position.x;
        const float blockWidth = block.size.x;

        if ( ((blockPositionX + blockWidth + offset >= screenWidth) && (speed > 0)) 
        || ((blockPositionX + offset <= 0) && (speed < 0)) ) 
        {
            block.craneBlockSpeed = -block.craneBlockSpeed;
        }
        else
        {
            block.position.x = blockPositionX + offset;
            block.shape.setPosition(block.position);
        }
    }
}

void updateFallingBlock(sf::VideoMode desktopMode, Block& block, vector<Block>& building, GameEvents& gameEvents) {
    if (gameEvents.isBlockFalling) {
        float dTime = gameEvents.dTime;
        const float speed = block.fallenBlockSpeed;
        const float offset = speed * dTime;

        const float blockPositionY = block.position.y;
        const float blockHeight = block.size.y;
        const float buildingHeight = desktopMode.height - building.size() * blockHeight;

        if (blockPositionY + blockHeight + offset >= buildingHeight) {
            gameEvents.isBlockFalling = false;
            building.push_back(block);
            gameEvents.isNewBlock = true;
        }
        block.position.y = blockPositionY + offset;
        block.shape.setPosition(block.position);
    }
}


void pollEvents(sf::RenderWindow& window, GameEvents& gameEvents) {
  
    sf::Event event;
    while (window.pollEvent(event)){
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            if ((event.key.code == sf::Keyboard::Return) && (!gameEvents.isBlockFalling)) {
                gameEvents.isBlockFalling = true;
            }
        default:
            break;
        }
    }
}

void redrawFrame(sf::RenderWindow& window, Block& block, vector<Block>& building)
{
    window.clear();
    for (size_t i = 0; i < building.size(); i++) {
        window.draw(building[i].shape);
    }
    window.draw(block.shape);
    window.display();
}


void gameProcess(sf::VideoMode desktopMode, GameEvents& gameEvents, vector<Block>& building, Block& block) {
    updateBlockOnCrane(desktopMode, block, gameEvents);
    updateFallingBlock(desktopMode, block, building, gameEvents);    
}

void initStartBlock(sf::VideoMode desktopMode, vector<Block>& building) {
    Block startBlock;
    const sf::Vector2f basePosition = { 
        (static_cast<float>(desktopMode.width) - startBlock.size.x) / 2, 
        static_cast<float>(desktopMode.height) - startBlock.size.y
    };
    startBlock.position = basePosition;
    startBlock.shape.setSize(startBlock.size); 
    startBlock.shape.setPosition(basePosition); 
    startBlock.shape.setFillColor(sf::Color::Red); 
    
    building.push_back(startBlock);
}

int main() {

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(
        desktopMode, 
        "BRICKSTER",
        sf::Style::Default,
        settings
    );

    sf::Clock clock;
    GameEvents gameEvents; 
    vector<Block> building;

    initStartBlock(desktopMode, building);

    Block newBlock;

    while (window.isOpen()) {
        float dTime = clock.restart().asSeconds();
        gameEvents.dTime = dTime;

        if (gameEvents.isNewBlock) {
            newBlock = initBlock(); 
            gameEvents.isNewBlock = false;
        }

        pollEvents(window, gameEvents);
        gameProcess(desktopMode, gameEvents, building, newBlock);
        redrawFrame(window, newBlock, building);
    }

}