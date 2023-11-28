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
    sf::Vector2f size;

    float craneBlockSpeed = 1000.0f;
    float fallenBlockSpeed = 700.0f;
};

struct GameEvents
{
    sf::VideoMode desktopMode;
    bool isNewBlock = true;
    bool isBlockFalling = false;
    float dTime;
    float leftDropZone;
    float rightDropZone;
};

Block initBlock(Block& prevBlock) {
    Block block;
    block.size = prevBlock.size;
    block.shape.setSize(prevBlock.size); 
    block.shape.setPosition({0, 0}); 
    block.shape.setFillColor(sf::Color::Red);
    return block;
}

void updateBlockOnCrane(Block& block, GameEvents& gameEvents) {
    if (!gameEvents.isBlockFalling) { 
        float dTime = gameEvents.dTime;
        const float speed = block.craneBlockSpeed;
        const float offset = speed * dTime;

        const float screenWidth = gameEvents.desktopMode.width;
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

void collisionBlock(Block& block, GameEvents& gameEvents) {
    if (block.position.x < gameEvents.leftDropZone) {
        float delta = gameEvents.leftDropZone - block.position.x;
        sf::Vector2f newPosition = {gameEvents.leftDropZone, block.position.y};
        block.size.x = block.size.x - delta;
        block.shape.setSize(block.size);
        block.position = newPosition;
        block.shape.setPosition(newPosition);
        gameEvents.rightDropZone = gameEvents.leftDropZone + block.size.x;
    }
    else {
        float delta = block.position.x + block.size.x - gameEvents.rightDropZone;
        block.size.x = block.size.x - delta;
        block.shape.setSize(block.size);
        block.shape.setPosition(block.position);
        gameEvents.rightDropZone = block.position.x + block.size.x;
        gameEvents.leftDropZone = gameEvents.rightDropZone - block.size.x;
    }
}


void updateFallingBlock(Block& block, vector<Block>& building, GameEvents& gameEvents) {
    if (gameEvents.isBlockFalling) {
        float dTime = gameEvents.dTime;
        const float speed = block.fallenBlockSpeed;
        const float offset = speed * dTime;

        const float blockPositionY = block.position.y;
        const float blockHeight = block.size.y;
        const float blockWidth = block.size.x;
        const float buildingHeight = gameEvents.desktopMode.height - building.size() * blockHeight;

        block.position.y = blockPositionY + offset;
        if (blockPositionY + blockHeight + offset >= buildingHeight) {
            if (!((block.position.x + blockWidth <= gameEvents.leftDropZone) || (block.position.x >= gameEvents.rightDropZone))) {
                gameEvents.isBlockFalling = false;
                collisionBlock(block, gameEvents);
                building.push_back(block);
                gameEvents.isNewBlock = true;
            }    
        }
        if (block.position.y >= gameEvents.desktopMode.height) {
            block.size = {0,0};
            gameEvents.isBlockFalling = false;
            gameEvents.isNewBlock = true;
        }
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
            break;
        default:
            break;
        }
    }
}

void redrawFrame(sf::RenderWindow& window, Block& block, vector<Block>& building) {
    window.clear();

    for (size_t i = 0; i < building.size(); i++) {
        window.draw(building[i].shape);
    }

    window.draw(block.shape);
    window.display();
}


void gameProcess(GameEvents& gameEvents, vector<Block>& building, Block& block) {
    updateBlockOnCrane(block, gameEvents);
    updateFallingBlock(block, building, gameEvents);   
}

void initStartBlock(vector<Block>& building, GameEvents& gameEvents) {
    Block startBlock;
    const sf::Vector2f START_BLOCK_SIZE = { 250, 100 };
    startBlock.size = START_BLOCK_SIZE;
    const sf::Vector2f basePosition = { 
        (static_cast<float>(gameEvents.desktopMode.width) - startBlock.size.x) / 2, 
        static_cast<float>(gameEvents.desktopMode.height) - startBlock.size.y
    };
    startBlock.position = basePosition;
    startBlock.shape.setSize(startBlock.size); 
    startBlock.shape.setPosition(basePosition); 
    startBlock.shape.setFillColor(sf::Color::Red); 
    
    building.push_back(startBlock);
    
    gameEvents.leftDropZone = building[0].position.x;
    gameEvents.rightDropZone = building[0].position.x + building[0].size.x;
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
    gameEvents.desktopMode = desktopMode;
    vector<Block> building;

    initStartBlock(building, gameEvents);

    Block newBlock;

    while (window.isOpen()) {
        float dTime = clock.restart().asSeconds();
        gameEvents.dTime = dTime;

        if (gameEvents.isNewBlock) {
            newBlock = initBlock(building[building.size() - 1]);
            gameEvents.isNewBlock = false;
        }

        pollEvents(window, gameEvents);
        gameProcess(gameEvents, building, newBlock);
        redrawFrame(window, newBlock, building);
    }

}