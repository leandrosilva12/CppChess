#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

using namespace sf;
using namespace std;  

//Global variables
int windowWidth = 600;
int windowHeight = 600;
String pieces_names[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};
RenderWindow window(VideoMode(windowWidth, windowHeight), "CppChess");

// Função updateBoard 
void updateBoard(int board[8][8]) {  
    Texture texture;
    Sprite sprite;
    for (int x=0; x < 8; x++)
        for (int y=0; y < 8; y++){
            int p = board[x][y];
            if (p == 0) continue;
            
            if (!texture.loadFromFile("Images/" + pieces_names[abs(p)%7 - 1] + (p > 0 ? 'W' : 'B') + ".png")){
                cout << "0x1c0e6ae ERROR: Please reboot your pc and unnistall your operating system!" << endl;
            }
            sprite.setTexture(texture);
            sprite.setScale(    
                (float)windowWidth/sprite.getLocalBounds().width/8,
                (float)windowHeight/sprite.getLocalBounds().height/8);
            sprite.setPosition((float)windowWidth/sprite.getLocalBounds().width/8*x, (float)windowHeight/sprite.getLocalBounds().height/8*y);
            window.draw(sprite);
            window.display();
        }
}

int main(){
    
    int board[8][8] = {
        {-10,-2,-3,-12,-5,-3,-2,-10},
        {-1,-1,-1,-1,-1,-1,-1,-1},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 0, 0, 0, 0, 0, 0, 0, 0},
        { 1, 1, 1, 1, 1, 1, 1, 1}, 
        { 10, 2, 4, 12, 5, 4, 2, 10}
    };

    // Creates Window
    
    

    // Creates Object of type Event
    Event event;
    bool running = true;
    while (window.isOpen())
    {
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
                //  cout << "Lost Focus";
            // we don't process other types of events
            default:
                break;
        }

        window.clear(Color::White);

        // Loading texture
        Texture texture;
        if (!texture.loadFromFile("Images/board-brown.png")){
            cout << "0x1c0e6ae ERROR: Please reboot your pc and unnistall your operating system!" << endl;
        }

        Sprite sprite;
        sprite.setTexture(texture);
        sprite.setScale(    
            (float)windowWidth/sprite.getLocalBounds().width,
            (float)windowHeight/sprite.getLocalBounds().height);
        window.draw(sprite);
        window.display();

        updateBoard(board);

        window.setFramerateLimit(60);
    }

    return 0;
}
