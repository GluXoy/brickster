#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <string>

using namespace std;

struct Block
{
    sf::RectangleShape shape;
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Color darkenColor = sf::Color(200, 100, 100);
    float craneBlockSpeed = 550.0f;
    float fallenBlockSpeed = 700.0f;
};

struct BrokenBlockEvents {
    float brokenWidth = 0;
    Block brokenBlock;
    bool leftZone = false;
    bool rightZone = false;
};

struct GameEvents
{
    sf::VideoMode desktopMode;
    sf::RectangleShape darknessRect;
    bool isNewBlock = true;
    bool isBlockFalling = false;
    float dTime;
    float leftDropZone;
    float rightDropZone;
    sf::Texture startBlockTexture;
    sf::Texture brokenBlockTexture;
    sf::Texture blockTexture;
    sf::Texture heartTexture;
    BrokenBlockEvents brokenBlockEvents;
    int lifes = 3;
    sf::RectangleShape box;
    vector<sf::Sprite> hearts;
};

Block initBlock(Block& prevBlock, GameEvents& gameEvents) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> disX(500, gameEvents.desktopMode.width - 400 - static_cast<int>(prevBlock.size.x));

    Block block;
    block.size = prevBlock.size;
    block.shape.setSize(prevBlock.size); 
    block.shape.setTexture(&gameEvents.blockTexture);

    block.shape.setFillColor(block.darkenColor);

    const float randomX = static_cast<float>(disX(gen));
    block.position = {randomX, 230}; 

    block.shape.setPosition(block.position);
    return block;
}

void updateBlockOnCrane(Block& block, GameEvents& gameEvents) {
    if (gameEvents.isBlockFalling) { 
        return;
    }
    float dTime = gameEvents.dTime;
    const float speed = block.craneBlockSpeed;
    const float offset = speed * dTime;

    const float screenWidth = gameEvents.desktopMode.width - 290;
    const float blockPositionX = block.position.x;
    const float blockWidth = block.size.x;

    if ( ((blockPositionX + blockWidth + offset >= screenWidth) && (speed > 0)) 
    || ((blockPositionX + offset <= 400) && (speed < 0)) ) 
    {
        block.craneBlockSpeed = -block.craneBlockSpeed;
    }
    else
    {
        block.position.x = blockPositionX + offset;
        block.shape.setPosition(block.position);
    }
}

void initBrokenBlock(GameEvents& gameEvents, const float buildingHeight, float blockHeight) {
    Block block;
    block.position.y = buildingHeight;    
    float posX;
    if (gameEvents.brokenBlockEvents.leftZone) 
        posX = gameEvents.leftDropZone - gameEvents.brokenBlockEvents.brokenWidth;
    else  
        posX = gameEvents.rightDropZone;    
    block.position.x = posX;
    gameEvents.brokenBlockEvents.leftZone = false;
    gameEvents.brokenBlockEvents.rightZone = false;

    block.size.x = gameEvents.brokenBlockEvents.brokenWidth;
    block.size.y = blockHeight;
    block.shape.setSize(block.size);

    block.shape.setTexture(&gameEvents.brokenBlockTexture);
    block.shape.setFillColor(block.darkenColor);

    block.shape.setPosition(block.position);
    gameEvents.brokenBlockEvents.brokenBlock = block;
}

void collisionBlock(Block& block, GameEvents& gameEvents, const float buildingHeight) {
    float delta;
    if (block.position.x < gameEvents.leftDropZone) {
        delta = gameEvents.leftDropZone - block.position.x;
        sf::Vector2f newPosition = {gameEvents.leftDropZone, buildingHeight - block.size.y};
        block.size.x = block.size.x - delta;
        block.shape.setSize(block.size);
        block.position = newPosition;
        block.shape.setPosition(newPosition);
        gameEvents.rightDropZone = gameEvents.leftDropZone + block.size.x;
        gameEvents.brokenBlockEvents.leftZone = true;
    }
    else {
        delta = block.position.x + block.size.x - gameEvents.rightDropZone;
        block.size.x = block.size.x - delta;
        block.shape.setSize(block.size);
        block.position.y = buildingHeight - block.size.y;
        block.shape.setPosition(block.position);
        gameEvents.rightDropZone = block.position.x + block.size.x;
        gameEvents.leftDropZone = gameEvents.rightDropZone - block.size.x;
        gameEvents.brokenBlockEvents.rightZone = true;
    }
    gameEvents.brokenBlockEvents.brokenWidth = delta;
    initBrokenBlock(gameEvents, buildingHeight, block.size.y);
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
            block.shape.setFillColor(sf::Color::White);
            building.push_back(block);
            gameEvents.isNewBlock = true;
            return;
        }    
    }
    if (block.position.y >= gameEvents.desktopMode.height) {
        block.size = {0,0};
        gameEvents.isBlockFalling = false;
        gameEvents.isNewBlock = true;
        gameEvents.lifes--;
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

void redrawFrame(sf::RenderWindow& window, Block& block, vector<Block>& building, sf::Sprite& backgroundSprite, GameEvents& gameEvents) {
    window.clear();
    
    window.draw(backgroundSprite);
    window.draw(gameEvents.darknessRect);

    for (size_t i = 0; i < building.size(); i++) {
        window.draw(building[i].shape);
    }

    window.draw(gameEvents.box);
    for (size_t i = 0; i < gameEvents.lifes; i++) {
        window.draw(gameEvents.hearts[i]);
    }

    window.draw(block.shape);
    if (gameEvents.brokenBlockEvents.brokenWidth != 0)
        window.draw(gameEvents.brokenBlockEvents.brokenBlock.shape);
    
    // window.display();
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
    if (building.size() > 8) {
        float newBuildingHeight = building[0].size.y * building.size() - building[0].size.y;
        building.erase(building.begin());
        
        updateBuildingPositions(gameEvents, building, newBuildingHeight);
    }
}

void updateBrokenBlock(GameEvents& gameEvents) {
    if (gameEvents.brokenBlockEvents.brokenWidth == 0) return;
    float offset = gameEvents.dTime * gameEvents.brokenBlockEvents.brokenBlock.fallenBlockSpeed;
    gameEvents.brokenBlockEvents.brokenBlock.position.y += offset;
    gameEvents.brokenBlockEvents.brokenBlock.shape.setPosition(gameEvents.brokenBlockEvents.brokenBlock.position);
    if (gameEvents.brokenBlockEvents.brokenBlock.position.y >= gameEvents.desktopMode.height) {
        gameEvents.brokenBlockEvents.brokenBlock.size = {0,0};
        gameEvents.brokenBlockEvents.brokenWidth = 0;
        return;
    }
}

void gameProcess(GameEvents& gameEvents, vector<Block>& building, Block& block) {
    updateBlockOnCrane(block, gameEvents);
    updateFallingBlock(block, building, gameEvents); 
    updateBrokenBlock(gameEvents);
    updateBuildingHeight(building, gameEvents);  
}

void initStartBlock(vector<Block>& building, GameEvents& gameEvents) {
    if (!gameEvents.startBlockTexture.loadFromFile("bricks/wall8.png")) {
        cerr << "Failed to load startBlockTexture" << endl;
    }
    Block startBlock;
    const sf::Vector2f START_BLOCK_SIZE = { 190, 60 };
    startBlock.size = START_BLOCK_SIZE;
    startBlock.shape.setSize(startBlock.size); 

    const sf::Vector2f basePosition = { 
        (static_cast<float>(gameEvents.desktopMode.width) - startBlock.size.x) / 2, 
        static_cast<float>(gameEvents.desktopMode.height) - startBlock.size.y
    };

    startBlock.position = basePosition;
    startBlock.shape.setTexture(&gameEvents.startBlockTexture);
    startBlock.shape.setPosition(basePosition); 
    
    building.push_back(startBlock);
    
    gameEvents.leftDropZone = building[0].position.x;
    gameEvents.rightDropZone = building[0].position.x + building[0].size.x;
}

void initHeart(GameEvents& gameEvents) {
    sf::Sprite heart;
    const sf::Vector2f size = {80.0f, 80.0f};
    heart.setTexture(gameEvents.heartTexture);
    heart.setScale(size.x / gameEvents.heartTexture.getSize().x, size.y / gameEvents.heartTexture.getSize().y);
    gameEvents.hearts.push_back(heart);
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

    initStartBlock(building, gameEvents);
    initLifesBox(gameEvents);
    initBrokenBlockTexture(gameEvents);
    initBlockTexture(gameEvents);
    
    Block newBlock;

    sf::Texture ctexture;
    if (!ctexture.loadFromFile("other/crane.png")) {
        cerr << "Failed to load crane texture" << endl;
        return -1;
    }
    sf::Sprite crane;
    const sf::Vector2f size = {1920.0f, 1080.0f};
    crane.setTexture(ctexture);
    crane.setScale(size.x / ctexture.getSize().x, size.y / ctexture.getSize().y);

    crane.setPosition({ 100, 0});

    sf::Texture hookTexture;
    if (!hookTexture.loadFromFile("other/hook.png")) {
        cerr << "Failed to load hook texture" << endl;
        return -1;
    }
    sf::Sprite hook;
    const sf::Vector2f sizeHook = { 130.0f, 150.0f };
    hook.setTexture(hookTexture);
    hook.setScale(sizeHook.x / hookTexture.getSize().x, sizeHook.y / hookTexture.getSize().y);

    hook.setPosition({ 900, 135});


    while (window.isOpen() && gameEvents.lifes) {
        float dTime = clock.restart().asSeconds();
        gameEvents.dTime = dTime;

        if (gameEvents.isNewBlock) {
            newBlock = initBlock(building[building.size() - 1], gameEvents);
            gameEvents.isNewBlock = false;
        }

        pollEvents(window, gameEvents);
        gameProcess(gameEvents, building, newBlock);
        redrawFrame(window, newBlock, building, backgroundSprite, gameEvents);
        window.draw(crane);
        window.draw(hook);
        window.display();
    }

}