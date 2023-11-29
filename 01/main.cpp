#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>

using namespace std;

struct Block
{
    sf::RectangleShape shape;
    sf::Vector2f position; //заменить позже на константу начальных координат появляющегося блока на кране
    sf::Vector2f size;
    sf::Color color = sf::Color(120, 35, 10);

    float craneBlockSpeed = 700.0f;
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
    sf::Sprite background;
};

Block initBlock(Block& prevBlock, const sf::Texture& blockTexture, GameEvents& gameEvents) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> disX(0, gameEvents.desktopMode.width - static_cast<int>(prevBlock.size.x));

    Block block;
    block.size = prevBlock.size;
    block.shape.setSize(prevBlock.size); 
    block.shape.setTexture(&blockTexture);

    // Генерация случайной позиции по оси X
    const float randomX = static_cast<float>(disX(gen));
    
    // Позиция блока с учетом случайной координаты X
    block.position = {randomX, 0}; 

    block.shape.setPosition(block.position);
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

void collisionBlock(Block& block, GameEvents& gameEvents, const float buildingHeight) {
    if (block.position.x < gameEvents.leftDropZone) {
        float delta = gameEvents.leftDropZone - block.position.x;
        sf::Vector2f newPosition = {gameEvents.leftDropZone, buildingHeight - block.size.y};
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
        block.position.y = buildingHeight - block.size.y;
        block.shape.setPosition(block.position);
        gameEvents.rightDropZone = block.position.x + block.size.x;
        gameEvents.leftDropZone = gameEvents.rightDropZone - block.size.x;
    }
}


void updateFallingBlock(Block& block, vector<Block>& building, GameEvents& gameEvents) {
    if (!gameEvents.isBlockFalling) {
        return;
    }
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
            collisionBlock(block, gameEvents, buildingHeight);
            building.push_back(block);
            gameEvents.isNewBlock = true;
            return;
        }    
    }
    if (block.position.y >= gameEvents.desktopMode.height) {
        block.size = {0,0};
        gameEvents.isBlockFalling = false;
        gameEvents.isNewBlock = true;
        return;
    }
    block.shape.setPosition(block.position);
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
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            } else if ((event.key.code == sf::Keyboard::Return) && (!gameEvents.isBlockFalling)) {
                gameEvents.isBlockFalling = true;
            }
            break;
        default:
            break;
        }
    }
}

void redrawFrame(sf::RenderWindow& window, Block& block, vector<Block>& building, sf::Sprite& backgroundSprite) {
    window.clear();
    
    window.draw(backgroundSprite);
    sf::RectangleShape darknessRect(sf::Vector2f(window.getSize().x, window.getSize().y));
    darknessRect.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(0.5 * 255)));
    window.draw(darknessRect);

    for (size_t i = 0; i < building.size(); i++) {
        window.draw(building[i].shape);
    }

    window.draw(block.shape);
    window.display();
}

void updateBuildingPositions(GameEvents& gameEvents, vector<Block>& building, float newBuildingHeight) {
    float currentHeight = gameEvents.desktopMode.height - newBuildingHeight;
    
    for (size_t i = building.size(); i > 0; i--) {
        building[i - 1].position.y = currentHeight;
        building[i - 1].shape.setPosition(building[i - 1].position);
        currentHeight += building[i - 1].size.y;
    }
}

void updateBuildingHeight(vector<Block>& building, GameEvents& gameEvents) {
    if (building.size() > 7) {
        float newBuildingHeight = building[0].size.y * building.size() - building[0].size.y;
        building.erase(building.begin());
        
        updateBuildingPositions(gameEvents, building, newBuildingHeight);
    }
}

void gameProcess(GameEvents& gameEvents, vector<Block>& building, Block& block) {
    updateBlockOnCrane(block, gameEvents);
    updateFallingBlock(block, building, gameEvents); 
    updateBuildingHeight(building, gameEvents);  
}

void initStartBlock(vector<Block>& building, GameEvents& gameEvents) {
    Block startBlock;
    const sf::Vector2f START_BLOCK_SIZE = { 250, 100 };
    startBlock.size = START_BLOCK_SIZE;
    startBlock.shape.setSize(startBlock.size); 

    const sf::Vector2f basePosition = { 
        (static_cast<float>(gameEvents.desktopMode.width) - startBlock.size.x) / 2, 
        static_cast<float>(gameEvents.desktopMode.height) - startBlock.size.y
    };

    startBlock.position = basePosition;
    startBlock.shape.setPosition(basePosition); 
    
    building.push_back(startBlock);
    
    gameEvents.leftDropZone = building[0].position.x;
    gameEvents.rightDropZone = building[0].position.x + building[0].size.x;
}


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
    vector<Block> building;

    sf::Texture startBlockTexture;
    if (!startBlockTexture.loadFromFile("bricks/wall2.png")) {
        cerr << "Failed to load texture" << endl;
        return -1;
    }
    initStartBlock(building, gameEvents);
    building[0].shape.setTexture(&startBlockTexture);

    Block newBlock;

    while (window.isOpen()) {
        float dTime = clock.restart().asSeconds();
        gameEvents.dTime = dTime;

        if (gameEvents.isNewBlock) {
            newBlock = initBlock(building[building.size() - 1], startBlockTexture, gameEvents);
            gameEvents.isNewBlock = false;
        }

        pollEvents(window, gameEvents);
        gameProcess(gameEvents, building, newBlock);
        redrawFrame(window, newBlock, building, backgroundSprite);
    }

}