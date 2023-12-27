#include "Structures.hpp"
#include "GameLogic.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

Block initBlock(Block& prevBlock, GameEvents& gameEvents) {
    Block block;
    block.size = prevBlock.size;
    block.shape.setSize(prevBlock.size); 
    block.shape.setTexture(&gameEvents.assets.blockTexture);
    block.shape.setFillColor(gameEvents.blockParams.craneBlockColor);

    float posX = 410.0f;
    if (gameEvents.state.blockCounter % 2 == 0) {
        posX = 1500.0f;
        gameEvents.blockParams.craneBlockSpeed = -gameEvents.blockParams.craneBlockSpeed;
    }

    block.position = {posX, 230}; 
    block.shape.setPosition(block.position);

    gameEvents.visuals.hook.position.x = (block.position.x + block.size.x / 2) - float(gameEvents.visuals.hook.shape.getGlobalBounds().width / 2);
    gameEvents.visuals.hook.shape.setPosition(gameEvents.visuals.hook.position);
    
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
    if (gameEvents.state.isBlockFalling || gameEvents.state.isShowScoreAnimation) { 
        return;
    }
    float dTime = gameEvents.timers.dTime;
    const float speed = gameEvents.blockParams.craneBlockSpeed;
    const float offset = speed * dTime;

    const float screenWidth = gameEvents.desktopMode.width - 290;
    const float blockPositionX = block.position.x;
    const float blockWidth = block.size.x;

    if ( ((blockPositionX + blockWidth + offset >= screenWidth) && (speed > 0)) 
    || ((blockPositionX + offset <= 400) && (speed < 0)) ) 
    {
        gameEvents.blockParams.craneBlockSpeed = -gameEvents.blockParams.craneBlockSpeed;
    }
    else
    {
        block.position.x = blockPositionX + offset;
        block.shape.setPosition(block.position);
        gameEvents.visuals.hook.position.x += offset;
        gameEvents.visuals.hook.shape.setPosition(gameEvents.visuals.hook.position);
    }
}

void initBrokenBlock(GameEvents& gameEvents, const float buildingHeight, float blockHeight) {
    Block block;
    block.position.y = buildingHeight;    
    float posX;
    if (gameEvents.state.brokenBlockEvents.leftZone) 
        posX = gameEvents.state.leftDropZone - gameEvents.state.brokenBlockEvents.brokenWidth;
    else  
        posX = gameEvents.state.rightDropZone;    
    block.position.x = posX;
    gameEvents.state.brokenBlockEvents.leftZone = false;
    gameEvents.state.brokenBlockEvents.rightZone = false;

    block.size.x = gameEvents.state.brokenBlockEvents.brokenWidth;
    block.size.y = blockHeight;
    block.shape.setSize(block.size);

    block.shape.setTexture(&gameEvents.assets.brokenBlockTexture);
    block.shape.setFillColor(gameEvents.blockParams.craneBlockColor);

    block.shape.setPosition(block.position);
    gameEvents.state.brokenBlockEvents.brokenBlock = block;
}

void initSuccesfulFlash(GameEvents& gameEvents, sf::Vector2f blockSize, sf::Vector2f position, vector<Block>& building) {
    const sf::Vector2f size = {blockSize.x * 2, blockSize.y / 2};
    gameEvents.visuals.flash.setScale(size.x / gameEvents.assets.flashTexture.getSize().x, size.y / gameEvents.assets.flashTexture.getSize().y);
    if (building.size() >= gameEvents.state.maxViewBlocks) 
        gameEvents.visuals.flash.setPosition(position.x - blockSize.x / 2, position.y + 2 * blockSize.y - size.y / 2);
    else
        gameEvents.visuals.flash.setPosition(position.x - blockSize.x / 2, position.y + blockSize.y - size.y / 2);
    gameEvents.state.showFlash = true;
}

void changeBlockCraneSpeed(GameEvents& gameEvents) {
    if (gameEvents.state.blockCounter % 7 == 0) {
        if (gameEvents.blockParams.craneBlockSpeed > 0)
            gameEvents.blockParams.craneBlockSpeed += gameEvents.blockParams.startCraneBlockSpeed * 0.05;
        else
            gameEvents.blockParams.craneBlockSpeed -= gameEvents.blockParams.startCraneBlockSpeed * 0.05;
    }
}

void initAntiBonus(GameEvents& gameEvents, Block& block, float buildingHeight) {
    if (gameEvents.visuals.bomb.list[0] != gameEvents.state.blockCounter) return;
    gameEvents.visuals.bomb.list.erase(gameEvents.visuals.bomb.list.begin());
    sf::Vector2f size = gameEvents.visuals.bomb.size;
    sf::Vector2f position;
    position.y = buildingHeight - size.y - 5.0f;
    if (gameEvents.state.blockCounter < gameEvents.state.maxViewBlocks) position.y -= block.size.y;
    position.x = block.position.x + block.size.x / 2 + size.x / 2;
    gameEvents.visuals.bomb.position = position;
    gameEvents.visuals.bomb.shape.setPosition(gameEvents.visuals.bomb.position);
    float textureRectX = gameEvents.visuals.bomb.textureRectSize.x;
    float textureRectY = gameEvents.visuals.bomb.textureRectSize.y;
    gameEvents.visuals.bomb.shape.setScale(size.x / textureRectX, size.y / textureRectY);

    sf::IntRect textureRect(0, 0, textureRectX, textureRectY);
    gameEvents.visuals.bomb.shape.setTextureRect(textureRect);
    gameEvents.timers.bombTimer.restart();
    gameEvents.visuals.bomb.currentFrame = 0.0f;
    gameEvents.state.isShowAntiBonusAnimation = true;
}

void collisionBlock(Block& block, GameEvents& gameEvents, const float buildingHeight, vector<Block>& building) {
    changeBlockCraneSpeed(gameEvents);
    float delta;
    float ratio = block.size.x / gameEvents.blockParams.startSize.x;
    float tanhResult = tanh(1 - ratio);
    float slowdownFactor = (1 - ratio) * 0.55; 
    float curUnbrokenPercent = gameEvents.state.unbrokenPercent + (1 - gameEvents.state.unbrokenPercent) * (slowdownFactor * tanhResult);
    
    if (block.position.x < gameEvents.state.leftDropZone) {
        delta = gameEvents.state.leftDropZone - block.position.x;
        sf::Vector2f newPosition = {gameEvents.state.leftDropZone, buildingHeight - block.size.y};
        block.position = newPosition;

        if ((delta < block.size.x * curUnbrokenPercent) || (block.size.x <= gameEvents.blockParams.minWidth)) {
            block.shape.setPosition(newPosition);
            initSuccesfulFlash(gameEvents, block.size, block.position, building);
            return;
        }

        if (block.size.x - delta <= gameEvents.blockParams.minWidth) {
            delta = block.size.x - gameEvents.blockParams.minWidth;
            block.size.x = gameEvents.blockParams.minWidth;
        }
        else
            block.size.x = block.size.x - delta;

        block.shape.setSize(block.size);
        block.shape.setPosition(newPosition);
        gameEvents.state.rightDropZone = gameEvents.state.leftDropZone + block.size.x;
        gameEvents.state.brokenBlockEvents.leftZone = true;
    }
    else {
        delta = block.position.x + block.size.x - gameEvents.state.rightDropZone;
        block.position.y = buildingHeight - block.size.y;
        if ((delta < block.size.x * curUnbrokenPercent) || (block.size.x <= gameEvents.blockParams.minWidth)) {
            block.position.x = gameEvents.state.leftDropZone;
            block.shape.setPosition(block.position);
            initSuccesfulFlash(gameEvents, block.size, block.position, building);
            return;
        }

        if (block.size.x - delta <= gameEvents.blockParams.minWidth) {
            delta = block.size.x - gameEvents.blockParams.minWidth;
            block.size.x = gameEvents.blockParams.minWidth;
            block.position.x = gameEvents.state.rightDropZone - gameEvents.blockParams.minWidth;
        }
        else
            block.size.x = block.size.x - delta;

        block.shape.setSize(block.size);
        block.shape.setPosition(block.position);
        gameEvents.state.rightDropZone = block.position.x + block.size.x;
        gameEvents.state.leftDropZone = gameEvents.state.rightDropZone - block.size.x;
        gameEvents.state.brokenBlockEvents.rightZone = true;
    }
    gameEvents.state.brokenBlockEvents.brokenWidth = delta;
    initBrokenBlock(gameEvents, buildingHeight, block.size.y);
}

void initExplosion(GameEvents& gameEvents, Block& block, float buildingHeight) {   
    gameEvents.state.isShowExplosion = true; 
    gameEvents.visuals.explosion.setTexture(gameEvents.assets.explosionTexture);
    sf::Vector2f size = {400, 400};
    gameEvents.visuals.explosion.setScale(size.x / gameEvents.assets.explosionTexture.getSize().x, size.y / gameEvents.assets.explosionTexture.getSize().y);
    sf::Vector2f position = {gameEvents.state.leftDropZone + block.size.x / 2 - size.x / 2, buildingHeight - 1.5f * block.size.y};
    gameEvents.visuals.explosion.setPosition(position);
    gameEvents.timers.explosionTimer.restart();
}

void updateBuildingAfterAntiBonus(GameEvents& gameEvents, vector<Block>& building) {
    for (int i = 1; i < 3 + 1; i++)
        building.erase(building.begin());
    float newBuildingHeight = building[0].size.y * building.size();
    updateBuildingPositions(gameEvents, building, newBuildingHeight);
}

bool collisionAntiBonus(GameEvents& gameEvents, Block& block, float buildingHeight) { 
    if (block.position.x + block.size.x < gameEvents.blockParams.startPosition.x + 30.0f) return false;
    if (block.position.x > gameEvents.blockParams.startPosition.x + gameEvents.blockParams.startSize.x - 30.0f) return false;
    initExplosion(gameEvents, block, buildingHeight);
    block.shape.setSize({0,0});
    gameEvents.state.craneCable.size.y = 0;  
    gameEvents.state.craneCable.shape.setSize(gameEvents.state.craneCable.size);
    gameEvents.state.isBlockFalling = false;
    gameEvents.state.isNewBlock = true;
    gameEvents.state.isShowAntiBonusAnimation = false;
    gameEvents.state.blockCounter -= 3;
    gameEvents.visuals.scoreText.setString("Score: " + to_string(gameEvents.state.blockCounter));
    return true;
}

void updateFallingBlock(Block& block, vector<Block>& building, GameEvents& gameEvents) {
    if (!gameEvents.state.isBlockFalling) {
        return;
    }
    float dTime = gameEvents.timers.dTime;
    const float speed = gameEvents.blockParams.fallenBlockSpeed;
    const float offset = speed * dTime;

    const float blockPositionY = block.position.y;
    const float blockHeight = block.size.y;
    const float blockWidth = block.size.x;
    const float buildingHeight = gameEvents.desktopMode.height - building.size() * blockHeight;
   
    block.position.y = blockPositionY + offset;

    gameEvents.state.craneCable.size.y += offset;  
    gameEvents.state.craneCable.shape.setSize(gameEvents.state.craneCable.size);

    if ((gameEvents.state.isShowAntiBonusAnimation) && (blockPositionY + blockHeight + offset >= buildingHeight - gameEvents.visuals.bomb.size.y / 3)) {
        if (collisionAntiBonus(gameEvents, block, buildingHeight) == true) {
            updateBuildingAfterAntiBonus(gameEvents, building);
            return;
        }
    }

    if (blockPositionY + blockHeight + offset >= buildingHeight) {
        if (!((block.position.x + blockWidth <= gameEvents.state.leftDropZone) || (block.position.x >= gameEvents.state.rightDropZone))) {
            gameEvents.state.isBlockFalling = false;
            collisionBlock(block, gameEvents, buildingHeight, building);
            block.shape.setFillColor(gameEvents.blockParams.buildingColor);
            building.push_back(block);
            initAntiBonus(gameEvents, block, buildingHeight);

            gameEvents.state.isShowScoreAnimation = true;
            gameEvents.state.blockCounter++;
            gameEvents.visuals.animatedScoreText.setString(to_string(gameEvents.state.blockCounter)); 
            gameEvents.visuals.scoreText.setString("Score: " + to_string(gameEvents.state.blockCounter));

            gameEvents.state.isNewBlock = true;
            gameEvents.state.craneCable.size.y = 0;
            gameEvents.state.craneCable.shape.setSize(gameEvents.state.craneCable.size);
            return;
        }    
    }
    if (block.position.y >= gameEvents.desktopMode.height) {
        block.size = {0,0};
        gameEvents.state.isBlockFalling = false;
        gameEvents.state.isNewBlock = true;
        gameEvents.state.lifes--;
        gameEvents.state.craneCable.size.y = 0;
        gameEvents.state.craneCable.shape.setSize(gameEvents.state.craneCable.size);
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
            } else if ((event.key.code == sf::Keyboard::Return) && (!gameEvents.state.isBlockFalling) && (!gameEvents.state.isShowScoreAnimation)) {
                sf::Vector2f craneCableposition = block.position;
                craneCableposition.x += block.size.x / 2;
                craneCableposition.y = gameEvents.visuals.hook.position.y + gameEvents.visuals.hook.size.y - 12;
                initCraneCable(gameEvents.state.craneCable, craneCableposition);
                gameEvents.state.isBlockFalling = true;
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
    window.draw(gameEvents.visuals.darknessRect);

    window.draw(gameEvents.visuals.scoreRectangle);
    window.draw(gameEvents.visuals.scoreText);
    window.draw(gameEvents.visuals.crane);
    for (size_t i = 0; i < building.size(); i++) {
        window.draw(building[i].shape);
    }

    window.draw(gameEvents.visuals.hook.shape);
    window.draw(block.shape);

    if (gameEvents.state.showFlash && gameEvents.timers.flashTimer.getElapsedTime().asSeconds() < 0.115) {
        window.draw(gameEvents.visuals.flash);
    } else {
        gameEvents.state.showFlash = false;
        gameEvents.timers.flashTimer.restart();
    }

    window.draw(gameEvents.state.craneCable.shape);    
    window.draw(gameEvents.visuals.box);
    for (size_t i = 0; i < gameEvents.state.lifes; i++) {
        window.draw(gameEvents.visuals.hearts[i]);
    }

    if (gameEvents.state.brokenBlockEvents.brokenWidth != 0)
        window.draw(gameEvents.state.brokenBlockEvents.brokenBlock.shape);

    if (gameEvents.state.isShowScoreAnimation && gameEvents.timers.scoreTimer.getElapsedTime().asSeconds() <= gameEvents.timers.scoreAnimationDuration * 1.1) {
        window.draw(gameEvents.visuals.animatedScoreText);
    } else {
        gameEvents.state.isShowScoreAnimation = false;
        gameEvents.visuals.animatedScoreText.setCharacterSize(0);
        gameEvents.timers.scoreTimer.restart();
    }

    if (gameEvents.state.isShowAntiBonusAnimation && gameEvents.timers.bombTimer.getElapsedTime().asSeconds() < gameEvents.timers.bombAnimationDuration) {
        if (gameEvents.timers.bombTimer.getElapsedTime().asSeconds() >= gameEvents.timers.bombAnimationDelay) 
            window.draw(gameEvents.visuals.bomb.shape);
    } else {
        gameEvents.state.isShowAntiBonusAnimation = false;
    }

    if (gameEvents.state.isShowExplosion && gameEvents.timers.explosionTimer.getElapsedTime().asSeconds() < gameEvents.timers.explosionAnimationDuration)
        window.draw(gameEvents.visuals.explosion);
    else
        gameEvents.state.isShowExplosion = false;

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
    if (building.size() > gameEvents.state.maxViewBlocks) {
        float newBuildingHeight = building[0].size.y * building.size() - building[0].size.y;
        building.erase(building.begin());
        
        updateBuildingPositions(gameEvents, building, newBuildingHeight);
    }
}

void updateBrokenBlock(GameEvents& gameEvents) {
    if (gameEvents.state.brokenBlockEvents.brokenWidth == 0) return;
    float offset = gameEvents.timers.dTime * gameEvents.blockParams.fallenBlockSpeed;
    gameEvents.state.brokenBlockEvents.brokenBlock.position.y += offset;
    gameEvents.state.brokenBlockEvents.brokenBlock.shape.setPosition(gameEvents.state.brokenBlockEvents.brokenBlock.position);
    if (gameEvents.state.brokenBlockEvents.brokenBlock.position.y >= gameEvents.desktopMode.height) {
        gameEvents.state.brokenBlockEvents.brokenBlock.size = {0,0};
        gameEvents.state.brokenBlockEvents.brokenWidth = 0;
        return;
    }
}

void updateScoreAnimation(GameEvents& gameEvents) {
    if (!gameEvents.state.isShowScoreAnimation) return;
    if (gameEvents.timers.scoreTimer.getElapsedTime().asSeconds() >= gameEvents.timers.scoreAnimationDuration) return;
    float koefficient = gameEvents.timers.scoreTimer.getElapsedTime().asSeconds() / gameEvents.timers.scoreAnimationDuration;
    int newFontSize = gameEvents.state.targetFontSize * koefficient;
    int alpha = static_cast<int>(225 * (1.0f - koefficient));
    gameEvents.visuals.animatedScoreText.setFillColor(sf::Color(225, 225, 225, alpha));
    gameEvents.visuals.animatedScoreText.setCharacterSize(newFontSize);

    if (gameEvents.state.blockCounter == 10 && !(gameEvents.state.isUpdateScoreAnimationPosFor10)) {
        int newPosX = gameEvents.visuals.animatedScoreText.getPosition().x - 65;
        gameEvents.visuals.animatedScoreText.setPosition(newPosX, gameEvents.visuals.animatedScoreText.getPosition().y);
        gameEvents.state.isUpdateScoreAnimationPosFor10 = true;
        gameEvents.state.targetFontSize -= 80;
    }
    if (gameEvents.state.blockCounter == 100 && !(gameEvents.state.isUpdateScoreAnimationPosFor100)) {
        int newPosX = gameEvents.visuals.animatedScoreText.getPosition().x - 65;
        gameEvents.visuals.animatedScoreText.setPosition(newPosX, gameEvents.visuals.animatedScoreText.getPosition().y);
        gameEvents.state.isUpdateScoreAnimationPosFor100 = true;
        gameEvents.state.targetFontSize -= 70;
    }       
}

void updateAntiBonusAnimation(GameEvents& gameEvents) {
    if (!gameEvents.state.isShowAntiBonusAnimation) return;
    float textureRectX = gameEvents.visuals.bomb.textureRectSize.x;
    float textureRectY = gameEvents.visuals.bomb.textureRectSize.y;
    float dTime = gameEvents.timers.dTime;
    gameEvents.visuals.bomb.currentFrame += (3 * dTime);
    if (gameEvents.visuals.bomb.currentFrame > 2) gameEvents.visuals.bomb.currentFrame -= 2; 
    sf::IntRect textureRect(int(gameEvents.visuals.bomb.currentFrame) * textureRectX, 0, textureRectX, textureRectY);
    gameEvents.visuals.bomb.shape.setTextureRect(textureRect);
}

void gameProcess(GameEvents& gameEvents, std::vector<Block>& building, Block& block) {
    updateBlockAndHookOnCrane(block, gameEvents);
    updateFallingBlock(block, building, gameEvents); 
    updateBrokenBlock(gameEvents);
    updateBuildingHeight(building, gameEvents);  
    updateScoreAnimation(gameEvents);
    updateAntiBonusAnimation(gameEvents);
}
