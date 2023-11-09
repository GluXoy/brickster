#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct Block
{
    sf::RectangleShape shape;
    sf::Vector2f position = { 300, 400 }; //заменить позже на константу начальных координат появляющегося блока на кране
    sf::Vector2f size = { 200, 100 };

    float craneBlockSpeed = 1100.0f;
    float fallenBlockSpeed = 600.0f;

    bool isFalling = false;
};

void initBlock(Block& block) {    
    block.shape.setSize(block.size); 
    block.shape.setPosition(block.position); 
    block.shape.setFillColor(sf::Color::Red); 
}

void gameProcess() {
    //
}

void updateBlockOnCrane(sf::VideoMode desktopMode, Block& block, float dTime) {
    if (!block.isFalling) { 
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

void updateFallingBlock(sf::VideoMode desktopMode, Block& block, float dTime) {
    if (block.isFalling) {
        const float speed = block.fallenBlockSpeed;
        const float offset = speed * dTime;

        const float screenHeight = desktopMode.height;
        const float blockPositionY = block.position.y;
        const float blockHeight = block.size.y;

        if (blockPositionY + blockHeight + offset >= screenHeight) {
            block.position = { 300, 400 }; // заменить позже на константу начальных координат появляющегося блока на кране
            block.isFalling = false;
        }
        else
        {
            block.position.y = blockPositionY + offset;
            block.shape.setPosition(block.position);
        }
    }
}


void pollEvents(sf::RenderWindow& window, Block& block) {
  
    sf::Event event;
    while (window.pollEvent(event)){
        switch (event.type)
        {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            if ((event.key.code == sf::Keyboard::Return) && (!block.isFalling)) {
                block.isFalling = true;
            }
        default:
            break;
        }
    }
}

void redrawFrame(sf::RenderWindow& window, Block& block)
{
    window.clear();
    window.draw(block.shape);
    window.display();
}

int main() {

    // const MAX_BLOCKS = 5;

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

    Block block;
    initBlock(block);
    // vector<Block> building;



    while (window.isOpen()) {
        float dTime = clock.restart().asSeconds();
        pollEvents(window, block);
        updateBlockOnCrane(desktopMode, block, dTime);
        updateFallingBlock(desktopMode, block, dTime);
        redrawFrame(window, block);
    }

}