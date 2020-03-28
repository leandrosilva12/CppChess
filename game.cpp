#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

using namespace sf;

int main(){

    // Creates Window
    int windowWidth = 600;
    int windowHeight = 600;
    RenderWindow window(VideoMode(windowWidth, windowHeight), "PyChess");


    // Creates Object of type Event
    Event event;
    bool running = true;
    while (window.isOpen()){
        // gets the current event
        window.pollEvent(event);
        // check the type of the event...
        switch (event.type){
            // window closed
            case Event::Closed:
                window.close();
                running = false;
                break;
            // key pressed
            case Event::KeyPressed:
                break;
            case Event::LostFocus:
                // myGame.pause();
                // std:: cout << "Lost Focus";
            // we don't process other types of events
            default:
                break;
        }


        window.clear(sf::Color::White);

        //Loading texture
        sf::Texture texture;
        if (!texture.loadFromFile("Images/board-brown.png")){
            std :: cout << "o chico é gay " << std :: endl;
        }
        sf::Sprite sprite;
        sprite.setTexture(texture);
        sprite.setScale(
            (float)windowWidth/sprite.getLocalBounds().width,
            (float)windowHeight/sprite.getLocalBounds().height);
        window.draw(sprite);
        window.display();
        
    }





    return 0;
}
