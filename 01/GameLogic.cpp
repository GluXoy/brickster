#include "Structures.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <random>
#include <string>

using namespace std;

Block initBlock(Block& prevBlock, GameEvents& gameEvents) {
    static std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> disX(500, gameEvents.desktopMode.width - 400 - static_cast<int>(prevBlock.size.x));

    Block block;
    block.size = prevBlock.size;
    block.shape.setSize(prevBlock.size); 
    block.shape.setTexture(&gameEvents.blockTexture);
    block.shape.setFillColor(block.craneBlockColor);

    const float randomX = static_cast<float>(disX(gen));
    block.position = {randomX, 230}; 
    block.shape.setPosition(block.position);

    gameEvents.hook.position.x = (block.position.x + block.size.x / 2) - float(gameEvents.hook.shape.getGlobalBounds().width / 2);
    gameEvents.hook.shape.setPosition(gameEvents.hook.position);
    
    return block;
}

void initCraneCable(CraneCable& craneCable, sf::Vector2f position) {
    craneCable.position = position;
    craneCable.size = {4, 0};
    craneCable.position.x -= craneCable.size.x / 2;  
    craneCable.shape.setSize(craneCable.size);
    craneCable.shape.setFillColor(craneCable.color);
    craneCable.shape.setPosition(craneCable.position);
}

void updateBlockAndHookOnCrane(Block& block, GameEvents& gameEvents) {
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
        gameEvents.hook.position.x += offset;
        gameEvents.hook.shape.setPosition(gameEvents.hook.position);
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
    block.shape.setFillColor(block.craneBlockColor);

    block.shape.setPosition(block.position);
    gameEvents.brokenBlockEvents.brokenBlock = block;
}

void initSuccesfulFlash(GameEvents& gameEvents, sf::Vector2f blockSize, sf::Vector2f position) {
    const sf::Vector2f size = {blockSize.x * 2, blockSize.y / 2};
    gameEvents.flash.setScale(size.x / gameEvents.flashTexture.getSize().x, size.y / gameEvents.flashTexture.getSize().y);
    if (gameEvents.blockCounter >= gameEvents.maxViewBlocks) 
        gameEvents.flash.setPosition(position.x - blockSize.x / 2, position.y + 2 * blockSize.y - size.y / 2);
    else
        gameEvents.flash.setPosition(position.x - blockSize.x / 2, position.y + blockSize.y - size.y / 2);
    gameEvents.showFlash = true;
}

void collisionBlock(Block& block, GameEvents& gameEvents, const float buildingHeight) {
    float delta;
    if (block.position.x < gameEvents.leftDropZone) {
        delta = gameEvents.leftDropZone - block.position.x;
        sf::Vector2f newPosition = {gameEvents.leftDropZone, buildingHeight - block.size.y};
        block.position = newPosition;
        if (delta < block.size.x * gameEvents.unbrokenPercent) {
            block.shape.setPosition(newPosition);
            initSuccesfulFlash(gameEvents, block.size, block.position);
            return;
        }
        block.size.x = block.size.x - delta;
        block.shape.setSize(block.size);
        block.shape.setPosition(newPosition);
        gameEvents.rightDropZone = gameEvents.leftDropZone + block.size.x;
        gameEvents.brokenBlockEvents.leftZone = true;
    }
    else {
        delta = block.position.x + block.size.x - gameEvents.rightDropZone;
        block.position.y = buildingHeight - block.size.y;
        if (delta < block.size.x * gameEvents.unbrokenPercent) {
            block.position.x = gameEvents.leftDropZone;
            block.shape.setPosition(block.position);
            initSuccesfulFlash(gameEvents, block.size, block.position);
            return;
        }
        block.size.x = block.size.x - delta;
        block.shape.setSize(block.size);
        block.shape.setPosition(block.position);
        gameEvents.rightDropZone = block.position.x + block.size.x;
        gameEvents.leftDropZone = gameEvents.rightDropZone - block.size.x;
        gameEvents.brokenBlockEvents.rightZone = true;
    }
    gameEvents.brokenBlockEvents.brokenWidth = delta;
    initBrokenBlock(gameEvents, buildingHeight, block.size.y);
}

void updateFallingBlock(Block& block, std::vector<Block>& building, GameEvents& gameEvents) {
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

    gameEvents.craneCable.size.y += offset;  
    gameEvents.craneCable.shape.setSize(gameEvents.craneCable.size);

    if (blockPositionY + blockHeight + offset >= buildingHeight) {
        if (!((block.position.x + blockWidth <= gameEvents.leftDropZone) || (block.position.x >= gameEvents.rightDropZone))) {
            gameEvents.isBlockFalling = false;
            collisionBlock(block, gameEvents, buildingHeight);
            block.shape.setFillColor(block.buildingColor);
            building.push_back(block);

            gameEvents.blockCounter++;
            gameEvents.scoreText.setString("Score: " + to_string(gameEvents.blockCounter * 10));

            gameEvents.isNewBlock = true;
            gameEvents.craneCable.size.y = 0;
            gameEvents.craneCable.shape.setSize(gameEvents.craneCable.size);
            return;
        }    
    }
    if (block.position.y >= gameEvents.desktopMode.height) {
        block.size = {0,0};
        gameEvents.isBlockFalling = false;
        gameEvents.isNewBlock = true;
        gameEvents.lifes--;
        gameEvents.craneCable.size.y = 0;
        gameEvents.craneCable.shape.setSize(gameEvents.craneCable.size);
        return;
    }
    block.shape.setPosition(block.position);
}

void pollEvents(sf::RenderWindow& window, GameEvents& gameEvents, Block& block) {
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
                sf::Vector2f craneCableposition = block.position;
                craneCableposition.x += block.size.x / 2;
                craneCableposition.y = gameEvents.hook.position.y + gameEvents.hook.size.y - 12;
                initCraneCable(gameEvents.craneCable, craneCableposition);
                gameEvents.isBlockFalling = true;
            }
            break;
        default:
            break;
        }
    }
}

void redrawFrame(sf::RenderWindow& window, Block& block, std::vector<Block>& building, sf::Sprite& backgroundSprite, GameEvents& gameEvents) {
    window.clear();
    
    window.draw(backgroundSprite);
    window.draw(gameEvents.darknessRect);
    window.draw(gameEvents.scoreRectangle);
    window.draw(gameEvents.scoreText);
    window.draw(gameEvents.crane);
    window.draw(gameEvents.hook.shape);
    for (size_t i = 0; i < building.size(); i++) {
        window.draw(building[i].shape);
    }
    window.draw(block.shape);

    if (gameEvents.showFlash && gameEvents.flashTimer.getElapsedTime().asSeconds() < 0.1) {
        window.draw(gameEvents.flash);
    } else {
        gameEvents.showFlash = false;
        gameEvents.flashTimer.restart();
    }

    window.draw(gameEvents.craneCable.shape);
    window.draw(gameEvents.box);
    for (size_t i = 0; i < gameEvents.lifes; i++) {
        window.draw(gameEvents.hearts[i]);
    }

    if (gameEvents.brokenBlockEvents.brokenWidth != 0)
        window.draw(gameEvents.brokenBlockEvents.brokenBlock.shape);
    
    window.display();
}

void updateBuildingPositions(GameEvents& gameEvents, std::vector<Block>& building, float newBuildingHeight) {
    float currentHeight = gameEvents.desktopMode.height - newBuildingHeight;
    
    for (size_t i = building.size(); i > 0; i--) {
        building[i - 1].position.y = currentHeight;
        building[i - 1].shape.setPosition(building[i - 1].position);
        currentHeight += building[i - 1].size.y;
    }
}

void updateBuildingHeight(std::vector<Block>& building, GameEvents& gameEvents) {
    if (building.size() > gameEvents.maxViewBlocks) {
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

void gameProcess(GameEvents& gameEvents, std::vector<Block>& building, Block& block) {
    updateBlockAndHookOnCrane(block, gameEvents);
    updateFallingBlock(block, building, gameEvents); 
    updateBrokenBlock(gameEvents);
    updateBuildingHeight(building, gameEvents);  
}
