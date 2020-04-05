#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>


using namespace sf;
using namespace std;  

// global variables
int windowWidth = 600;
int windowHeight = 600;
Sprite boardSprite;
String pieces_names[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};
RenderWindow window(VideoMode(windowWidth, windowHeight), "CppChess");
vector<Sprite> spritePieces;
bool turn = true;

// Textures 

Texture pawnB; 
Texture pawnW;
Texture knightB;
Texture knightW;
Texture rookB;
Texture rookW;
Texture bishopB;
Texture bishopW;
Texture queenB;
Texture queenW;
Texture kingB;
Texture kingW;


int board[8][8] = {
    {-11, -1,  0,  0,  0,  0,  1, 11},
    { -2, -1,  0,  0,  0,  0,  1,  2},
    { -3, -1,  0,  0,  0,  0,  1,  3},
    { -5, -1,  0,  0,  0,  0,  1,  5},
    {-13, -1,  0,  0,  0,  0,  1, 13},
    { -3, -1,  0,  0,  0,  0,  1,  3},
    { -2, -1,  0,  0,  0,  0,  1,  2}, 
    {-11, -1,  0,  0,  0,  0,  1, 11}
};

// prototype functions
vector<bool> makeMove(int x, int y, int n, int m);
vector<pair<int, int>> getMovesLegal(int x, int y);
map<pair<int, int>,vector<pair<int, int>>> getAllMovesLegal();
bool underAttack(vector<pair<int, int>> pos_pieces);
bool isCheck();
int isCheckmate();
vector<pair<int, int>> getMovesAtk(int x, int y);
vector<pair<int, int>> getMovesNeutral(int x, int y);
vector<pair<int, int>> getMoves(int x, int y);

void getSprites(){
    spritePieces.clear();
    
    for (int x=0; x < 8; x++)
        for (int y=0; y < 8; y++){
            Sprite sprite;
            int p = board[y][x];
            if (p == 0) continue;

            switch (abs(p) % 7 * (p > 0 ? 1 : -1)){
                case -1:
                    sprite.setTexture(pawnB);
                    break;
                case -2:
                    sprite.setTexture(knightB);
                    break;
                case -3:
                    sprite.setTexture(bishopB);
                    break;
                case -4:
                    sprite.setTexture(rookB);
                    break;
                case -5:
                    sprite.setTexture(queenB);
                    break;
                case -6:
                    sprite.setTexture(kingB);
                    break;      
                case 1:
                    sprite.setTexture(pawnW);
                    break;
                case 2:
                    sprite.setTexture(knightW);
                    break;
                case 3:
                    sprite.setTexture(bishopW);
                    break;
                case 4:
                    sprite.setTexture(rookW);
                    break;
                case 5:
                    sprite.setTexture(queenW);
                    break;
                case 6:
                    sprite.setTexture(kingW);
                    break;    
                
                default:
                    break;
            }
            sprite.setScale(    
                (float)windowWidth/sprite.getLocalBounds().width/8,
                (float)windowHeight/sprite.getLocalBounds().height/8);

            sprite.setPosition(
                (float)sprite.getGlobalBounds().width*y, 
                (float)sprite.getGlobalBounds().height*x);
            
            
            spritePieces.push_back(sprite);
        }   
}



// pawn = 1 / 8
// knight = 2 
// bishop = 3
// rook = 4 / 11
// queen = 5
// king = 6 / 13


vector<bool> makeMove(int x, int y, int n, int m) {
    /**
     *  Fix pieces' position and attributes after a move. 
     */
    bool captured=false, promoted=false;
    int other;
    if (board[n][m] != 0)
        captured = true;

    // Make move
    cout << "motherfucker: "<< x << ',' << y << endl;
    int piece = board[x][y];
    board[x][y] = 0;
    board[n][m] = piece;

    
    switch (abs(piece)%7) {
        case 1:
            // Set pawn's ``en_passant`` attribute to true
            if (m == 3 + turn)
                board[n][m] = 8 * (piece > 0 ? 1 : -1);
            // Remove enemy pawns that were captured en passent
            // Acknowledge capture
            other = board[n][piece > 0 ? m+1 : m-1];
            if (abs(other) == 8){
                board[n][piece > 0 ? m+1 : m-1] = 0;
                captured = true;
            }

           
            if (m == 7*(!turn)){
				promoted = true;
            }
            break;
        case 4:
            board[n][m] = 4*(piece > 0 ? 1 : -1);
            break;
        case 6:
            if (abs(piece) == 13){
                //swap rook with the king
                if (n == 2){
                    //int rook = board[0][7*turn];
                    board[0][7*turn] = 0;
                    board[3][7*turn] = 4*(piece > 0 ? 1 : -1);

                } else if(n == 6){
                    //int rook = board[7][7*turn];
                    board[7][7*turn] = 0;
                    board[5][7*turn] = 4* (piece > 0 ? 1 : -1);
                }
            }
            board[n][m] = 6* (piece > 0 ? 1 : -1); // king has moved
            break;
    }
    // Rechange enemy pawn's attribute ``en_passant`` to false
    for (int i = 0; i < 8; i++){
        other = board[x][3+(!turn)];
        if (abs(other)%7 == 1 && (other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1)) 
            board[x][3 + !turn] = piece > 0 ? -1 : 1;
    }
    return {captured, promoted};
}

vector<pair<int, int>> getMovesLegal(int x, int y) {
    /** 
     * Return a list with allowed moves for one piece. 
     */
    int temp_board[8][8];
    vector<pair<int, int>> moves_legal;
    vector<pair<int, int>> moves = getMoves(x,y); 

    // Check if ``moves`` are already legal
    int piece = board[x][y];
    board[x][y] = 0;
    if ((abs(piece) % 7) != 6 && !isCheck()) {
        moves_legal = moves;
    } 
     
    // Filter the moves that are legal
    else {
        board[x][y] = piece;
        
        // Verify moves that stop the check
        for(pair<int, int> m : moves){
            // Copy board to temp_board
            for (int i = 0; i < 8; i++)
                for (int j = 0; j <8; j++)
                    temp_board[i][j] = board[i][j];

            makeMove(x, y, m.first, m.second);

            if (!isCheck())
                moves_legal.insert(moves_legal.end(), m);

            // Copy temp_board to board
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    board[i][j] = temp_board[i][j];
        }
    }
    
    board[x][y] = piece;
    
    return moves_legal; 
}


map<pair<int, int>, vector<pair<int, int>>> getAllMovesLegal() {
    /** 
     * Return a dictionary with allowed moves for each piece. 
     */
    int other;
    map<pair<int, int>, vector<pair<int, int>>> all_moves_legal;
    
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            other = board[x][y];
            pair<int, int> temp{x,y};
			if ((other > 0 ? 1 : 0) == turn) {
                all_moves_legal[temp] = getMovesLegal(x, y);

            }
        
        }
	return all_moves_legal;
}



bool underAttack(vector<pair<int, int>> pos_pieces) {
    /** 
     * Check if any of the positions is being attacked.
     *
     * @param pos_pieces The set of positions.
     */
    int other;
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            other = board[x][y];
            if ((other > 0 ? 1 : 0) != turn){
                for (pair<int, int> pos : pos_pieces)
                    for (pair<int, int> pos_atk : getMovesAtk(x, y))
                        if (pos == pos_atk)
                            return true;
            }
        }
    }
    return false;
}

bool isCheck() {
    /**
     * Check if king is being attacked.
     */
    int other;

    // Find king currently playing
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++) {
            other = board[x][y];
            if ((abs(other) % 7) == 6 && (other > 0 ? 1 : 0) == turn)
                if (underAttack({{x, y}}))
                    return true;
        }
    return false;
}

int isCheckmate() {
    /**
     * Return state of the game (0: no checkmate, 1: checkmate, 2: stalemate).
     */
    int checkmate;
    if (isCheck()) {
        checkmate = 1;
        for (const auto m : getAllMovesLegal()) {
            if (m.second.size() != 0) {
                checkmate = 0;
                break;
            }
        
        }
    } else {
        checkmate = 0;
        for (const auto m : getAllMovesLegal()) {
            if (m.second.size() != 0) {
                checkmate = 2;
                break;
            }
        }
    }

    return checkmate;
}


vector<pair<int, int>> getMovesAtk(int x, int y) {
    /** 
     * Return moves that are hostile. 
     */
    int a, b, k, other;
    int piece = board[x][y];
    vector<pair<int, int>> moves_atk;

    switch (abs(piece)%7) {
        case 1:
            // diagonal capture
            b = piece > 0 ? -1 : 1;
            for (a = -1; a <= 1; a += 2)
                if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) {       // inner board
                    other = board[x + a][y + b];
                    if (other == 0) {
                        other = board[x + a][y];
                        if (other == 8)                                         // pawn en_passant
                            moves_atk.insert(moves_atk.end(), {x + a, y + b});
                    } else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1))    // different teams
                        moves_atk.insert(moves_atk.end(), {x + a, y + b});
                }            
            break;
        case 2:
            // L-shape movement
            for (a = -2; a <= 2; a++)
                for (b = -2; b <= 2; b++) {
                    if (abs(a) == abs(b) || a == 0 || b == 0) continue;
                    if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) { 
                        other = board[x + a][y + b];
                        if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1) || other == 0) {
                            moves_atk.insert(moves_atk.end(), {x + a, y + b});
                        }
                    }
                }
            break;
        case 3:
            // diagonal movement
            for (a = -1; a <= 1; a+=2)
                for (b = -1; b <= 1; b+=2) {
                    for (k = 1; 0 <= x + a*k &&  x + a*k < 8 && 0 <= y + b*k && y + b*k < 8; k++) {
                        other = board[x + a*k][y + b*k];
                        if (other == 0)                                             // if empty, add
                            moves_atk.insert(moves_atk.end(), {x + a*k, y + b*k});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) {    // if enemy, add and break
                            moves_atk.insert(moves_atk.end(), {x + a*k, y + b*k});
                            break;
                        } else 
                            break;
                    }
                }
            break;
        case 4:
            // cross movements
            for (a = -1; a <= 1; a++)
                for (b = -1; b <= 1; b++) {
                    if (abs(a) == abs(b)) continue;
                    for (k = 1; 0 <= x + a*k &&  x + a*k < 8 && 0 <= y + b*k && y + b*k < 8; k++) {
                        other = board[x + a*k][y + b*k];
                        if (other == 0)
                            moves_atk.insert(moves_atk.end(), {x + a*k, y + b*k});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) {
                            moves_atk.insert(moves_atk.end(), {x + a*k, y + b*k});
                            break;
                        } else
                            break;
                    }
                }
            break;
        case 5:
            // cross-diagonal movement
            for (a = -1; a <= 1; a++)
                for (b = -1; b <= 1; b++) {
                    if (a == 0 && b == 0) continue;
                    for (k = 1; 0 <= x + a*k &&  x + a*k < 8 && 0 <= y + b*k && y + b*k < 8; k++) {
                        other = board[x + a*k][y + b*k];
                        if (other == 0)
                            moves_atk.insert(moves_atk.end(), {x + a*k, y + b*k});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) {
                            moves_atk.insert(moves_atk.end(), {x + a*k, y + b*k});
                            break;
                        } else
                            break;
                    }
                }
            break;
        case 6:
            // one-cross-diagonal movement
            for (a = -1; a <= 1; a++)
                for (b = -1; b <= 1; b++) {
                    if (a == 0 && b == 0) continue;
                    if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) {
                        other = board[x + a][y + b];
                        if (other == 0)
                            moves_atk.insert(moves_atk.end(), {x + a, y + b});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) {
                            moves_atk.insert(moves_atk.end(), {x + a, y + b});
                            break;
                        } else
                            break;
                    }
                }
            break;
    }

    return moves_atk;
}

vector<pair<int, int>> getMovesNeutral(int x, int y) {
    vector<pair<int, int>> moves_neutral;
    int a, b, other;
    int piece = board[x][y];
    switch (abs(piece)%7) {
        case 1:        
            // one-two-squares advance movement
            for (int c = 1; c <= 2; c++) {
                b = c*(piece > 0 ? -1 : 1);
                if (0 <= y + b && y + b < 8){
                    other = board[x][y + b];
                    if (other == 0) {
                        moves_neutral.insert(moves_neutral.end(), {x, y + b});
                        if(y != 1 + 5*(piece > 0 ? 1 : 0)) break; // só anda 1 pá frente
                    } else break;
                } 
            }  
            break;
        case 6:
            if (piece == 6)
                break;

            // castle
            other = board[0][y]; // canto esquerdo
            if (other == 11){
                // no piece between this rook and king
                bool can_move = true;
                for (int i = 0; i < 3; i++)
                    if (board[1 + i][y] != 0){
                        can_move = false;
                        break;
                    }
                if (can_move){
                    // enemy attack range doesn't reach squares between this rook and king
                    vector<pair<int, int>> moves =  {{2, y}, {3, y}, {4, y}};
                    if (!underAttack(moves))
                        moves_neutral.insert(moves_neutral.end(),{2, y});
                }        
            }
            other = board[7][y]; // canto esquerdo
            if (other == 11){
                // no piece between this rook and king
                bool can_move = true;
                for (int i = 0; i < 3; i++)
                    if (board[6 - i][y] != 0){
                        can_move = false;
                        break;
                    }
                if (can_move){
                    // enemy attack range doesn't reach squares between this rook and king
                    vector<pair<int, int>> moves =  {{4, y}, {5, y}, {6, y}};
                    if (!underAttack(moves))
                        moves_neutral.insert(moves_neutral.end(),{6, y});
                }        
            }
            break;
    }
    return moves_neutral;
}

vector<pair<int, int>> getMoves(int x, int y) {
    /**
     *  Return attack and neutral (if existing) moves considering the chess' movement rules (this moves may not be allowed). 
	 */
    vector<pair<int, int>> moves;
    vector<pair<int, int>> neutral;
    int piece = board[x][y];
    switch (abs(piece)%7) {
        case 1:
        case 6:     
            moves = getMovesAtk(x, y);
            neutral = getMovesNeutral(x, y);
            moves.insert( moves.end(), neutral.begin(), neutral.end() );
            break;
        default:
            moves = getMovesAtk(x, y);
            break;
    }
    return moves;
}


// Função updateBoard 
void displayBoard(){     
    window.draw(boardSprite);
    for(int i=0;i<spritePieces.size();i++){
        window.draw(spritePieces[i]);
    }
    window.draw(spritePieces[0]);
    window.display();
    
}

void printBoard(){

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            printf("%3d ", board[i][j]);
        }
        cout << endl;
    }
    cout << endl;
}



int main() {
    if(!pawnB.loadFromFile("Images/pawnB.png") || !pawnW.loadFromFile("Images/pawnW.png") || !knightB.loadFromFile("Images/knightB.png") || !knightW.loadFromFile("Images/knightW.png") || !rookB.loadFromFile("Images/rookB.png") || !rookW.loadFromFile("Images/rookW.png") || !bishopB.loadFromFile("Images/bishopB.png") || !bishopW.loadFromFile("Images/bishopW.png") || !queenB.loadFromFile("Images/queenB.png") || !queenW.loadFromFile("Images/queenW.png") || !kingB.loadFromFile("Images/kingB.png") || !kingW.loadFromFile("Images/kingW.png")) { 
        cout << "0x1c0e6ae ERROR: Please reboot your pc and unnistall your operating system!" << endl;
    }
    // Creates Window
    // Creates Object of type Event
    Event event;
    bool running = true;
    Texture texture;
    if (!texture.loadFromFile("Images/board-brown.png")){
        cout << "0x1c0e6ae ERROR: Please reboot your pc and unnistall your operating system!" << endl;
    }

    boardSprite.setTexture(texture);
    boardSprite.setScale(
            (float)windowWidth/boardSprite.getLocalBounds().width,
            (float)windowHeight/boardSprite.getLocalBounds().height);

    getSprites();


    map<pair<int,int>,vector<pair<int,int>>> turnMoves = getAllMovesLegal();

    
    for (const auto m : turnMoves) {
        cout << m.first.first << ',' << m.first.second << ": ";

        for (pair<int, int> p : m.second) {
            cout << '('<<p.first << ',' << p.second << ") ";
        }
        cout << endl;
    }
        
    int oldX;
    int oldY;
    pair<int, int> mouseRectOffset;
    int index;
    int mouseX;
    int mouseY;
    bool mouseClicked = false;
    bool dragging = false;
    displayBoard();
    window.display();

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
            case Event::MouseButtonPressed:
                if(event.mouseButton.button==Mouse::Left){
                    mouseClicked = true;
                    for(int i=0;i<spritePieces.size();i++){
                        if(spritePieces[i].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)){
                            dragging=true;
                            index = i;
                            oldX = (int)((float)event.mouseButton.x/windowWidth*8);
                            oldY = (int)((float)event.mouseButton.y/windowHeight*8);
                            mouseRectOffset.first = event.mouseButton.x - spritePieces[i].getGlobalBounds().left - spritePieces[i].getOrigin().x;
                            mouseRectOffset.second = event.mouseButton.y - spritePieces[i].getGlobalBounds().top - spritePieces[i].getOrigin().y ;
                        }
                    }                    
                }
                break;
            case Event::MouseButtonReleased:
                if(event.mouseButton.button==Mouse::Left){ 
                    if(!mouseClicked){
                        break;
                    }
                    mouseClicked = false;
                    dragging = false;
                    //vector<pair<int,int>> moves_legal = turnMoves[pair<int,int>(spritePieces[index].getPosition().x,spritePieces[index].getPosition().y)];
                    //cout << spritePieces[index].getPosition().x << ',' << spritePieces[index].getPosition().y << endl;
                    vector<pair<int,int>> moves_legal = turnMoves[pair<int,int>(oldX,oldY)];
                    
                    cout << moves_legal.size() << endl;

                    pair<int,int> newPos = {(int)((float)mouseX/windowWidth*8),(int)((float)mouseY/windowHeight*8)};
                    cout << newPos.first << ',' << newPos.second << endl;
                    
                    if (find(moves_legal.begin(), moves_legal.end(), newPos) != moves_legal.end()) {
                        makeMove(oldX, oldY, newPos.first, newPos.second);
                        turn = !turn;
                        turnMoves=getAllMovesLegal();
                        spritePieces[index].setPosition((int)((float)mouseX/(float)windowWidth*8)*75,(int)((float)mouseY/(float)windowHeight*8)*75);
                        printBoard();
                        getSprites();
                    } else {
                        spritePieces[index].setPosition(oldX*windowWidth/8, oldY*windowHeight/8);
                    }

                }
                break;
            case Event::MouseMoved:
                mouseX = event.mouseMove.x;
                mouseY = event.mouseMove.y;
                break;

            default:
                break;
        }
        if (dragging == true)
        {
            spritePieces[index].setPosition(mouseX - mouseRectOffset.first, mouseY - mouseRectOffset.second);
            window.draw(spritePieces[index]);
            window.display();
           
        }
        
        displayBoard();
        window.display();
        // Loading texture
        window.setFramerateLimit(1000);
    }
    return 0;
}