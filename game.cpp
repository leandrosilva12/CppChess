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
bool turn = true;
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

// pawn = 1 / 8
// knight = 2 
// bishop = 3
// rook = 4 / 11
// queen = 5
// king = 6 / 13

vector<bool> makeMove(int x, int y, int n, int m) {
    // Fix pieces' position and attributes after a move.
    bool captured, promoted;
    int other;
    if (board[n][m] != 0)
        captured = true;
    // Make move
    int piece = board[x][y];
    board[x][y] = 0;
    board[n][m] = piece;
    switch (abs(piece)%7) {
        case 1:
            // Set pawn's ``en_passant`` attribute to true
            if (m == 3 + turn)
                board[n][m] = 8;
            // Remove enemy pawns that were captured en passent
            // Acknowledge capture
            other = board[n][piece > 0 ? m : -m];
            if (other == 8){
                // Pode estar mal
                if( turn){
                    board[n][m] = 0;
                } else {
                    board[n][-m]= 0;
                }
                captured = true;
            }
            // Acknowledge promotion
            if (m == 7*(!turn)){
				promoted = true;
            }
            break;
        case 4:
            board[n][m] = 4;
            break;
        case 6:
            if (piece == 13){
                //swap rook with the king
                if (n == 2){
                    int rook = board[0][7*turn];
                    board[0][7*turn] = 0;
                    board[3][7*turn] = rook;

                } else if(n == 6){
                    int rook = board[7][7*turn];
                    board[7][7*turn] = 0;
                    board[5][7*turn] = rook;
                }
            }
            board[n][m] = 6;
            break;
    }
    // Rechange enemy pawn's attribute ``en_passant`` to false
    for (int i = 0; i < 8; i++){
        other = board[x][3+(!turn)];
        if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1))
            board[x][3 + !turn] = piece > 0 ? 1 : -1;
    }
    return {captured, promoted};
}

vector<Vector2i> getMovesLegal(int x, int y) {
    // Return a list with allowed moves for one piece.
    
    vector<Vector2i> movesLegal;
    vector<Vector2i> moves = getMoves(x,y);
    int piece = board[x][y];
    board[x][y] = 0;
    if (abs(piece) % 7) == 6 && isCheck()){
        movesLegal = moves;
    }else{
        board[x][y] = piece;
        for(Vector2i v : moves){
            ...
        }
    }
    ...
}

vector<Vector2i> getAllMovesLegal() {
    // Return a dictionary with allowed moves for each piece.
    map<Vector2i, vector<Vector2i>> all_moves_legal;
    
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
            other = board[x][y];
			if ((other > 0 ? 1 : 0) == turn)
                all_moves_legal.insert(<Vector2i, vector<Vector2i>>({x, y}, getMovesLegal(x, y)));
    
	return all_moves_legal;
}


/*
def get_moves_legal(self, x, y):
		""" Return a list with allowed moves for one piece. """
		# Remove piece from ``board`` and verify if king is being attacked
		moves_legal = set()
		moves = self.get_moves(x, y)

		piece = self.board[x][y]
		self.board[x][y] = None

		assert piece is not None
		
		if piece[NAME] != 'K' and not self.check():
			moves_legal = moves
		else:
			self.board[x][y] = piece

			# Verify moves that stop the check
			for n, m in moves:
				if piece[NAME] == 'K':
					if piece[COLOR] == '0':
						pos_kings = [(n, m), self.pos_kings[1]]				
					else:
						pos_kings = [self.pos_kings[0], (n, m)]
				else:
					pos_kings = list(self.pos_kings)

				game = Game(self.turn, [list(row) for row in self.board], pos_kings)				
				game.make_move(x, y, n, m)

				if not game.check():
					moves_legal.add((n, m))

		self.board[x][y] = piece
		
		return moves_legal

	def get_all_moves_legal(self):
		""" Return a dictionary with allowed moves for each piece.

		:rtype: dict[(int, int), (int, int)]
		"""
		all_moves_legal = {}

		for x in range(8):
			for y in range(8):
				other = self.board[x][y]
				if other is not None and other[COLOR] == str(self.turn):
					all_moves_legal[(x, y)] = self.get_moves_legal(x, y)

		return all_moves_legal

*/

vector<Vector2i> getMovesAtk(int x, int y) {
    int a, b, k, other;
    int piece = board[x][y];
    vector<Vector2i> movesAtk;

    switch (abs(piece)%7) {
        case 1:
            // diagonal capture
            b = piece > 0 ? -1 : 1;
            for (a = -1; a <= 1; a += 2)
                if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) { // nao sai dos limites
                    other = board[x + b][y + b];
                    if (other == 0) { // está vazio
                        other = board[x + a][y];
                        if (other == 8) // peao en_passant
                            movesAtk.insert(movesAtk.end(), {x + a, y + b});
                    } else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) // são de teams diferentes
                        movesAtk.insert(movesAtk.end(), {x + a, y + b});
                }            
            break;
        case 2:
            // L-shape movement
            for (a = -2; a <= 2; a++)
                for (b = -2; b <= 2; b++) {
                    if (abs(a) == abs(b) || a == 0 || b == 0) continue;
                    if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) { // nao sai dos limites
                        other = board[x + a][y + b];
                        if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1) || other == 0) { // são de teams diferentes
                            movesAtk.insert(movesAtk.end(), {x + a, y + b});
                        }
                    }
                }
            break;
        case 3:
            // diagonal movement
            for (a = -1; a <= 1; a++)
                for (b = -1; b <= 1; b++)
                    for (k = 1; 0 <= x + a*k &&  x + a*k < 8 && 0 <= y + b*k && y + b*k < 8; k++) {
                        other = board[x + a*k][y + b*k];
                        if (other == 0)     // se vazio, adiciona
                            movesAtk.insert(movesAtk.end(), {x + a*k, y + b*k});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) { // se inimigo, adiciona e sai
                            movesAtk.insert(movesAtk.end(), {x + a*k, y + b*k});
                            break;
                        } else // sai
                            break;
                    }
            break;
        case 4:
            // cross movement
            for (a = -1; a <= 1; a++)
                for (b = -1; b <= 1; b++) {
                    if (abs(a) == abs(b)) continue;
                    for (k = 1; 0 <= x + a*k &&  x + a*k < 8 && 0 <= y + b*k && y + b*k < 8; k++) {
                        other = board[x + a*k][y + b*k];
                        if (other == 0)     // se vazio, adiciona
                            movesAtk.insert(movesAtk.end(), {x + a*k, y + b*k});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) { // se inimigo, adiciona e sai
                            movesAtk.insert(movesAtk.end(), {x + a*k, y + b*k});
                            break;
                        } else // sai
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
                        if (other == 0)     // se vazio, adiciona
                            movesAtk.insert(movesAtk.end(), {x + a*k, y + b*k});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) { // se inimigo, adiciona e sai
                            movesAtk.insert(movesAtk.end(), {x + a*k, y + b*k});
                            break;
                        } else // sai
                            break;
                    }
                }
            break;
        case 6:
            // cross-diagonal movement
            for (a = -1; a <= 1; a++)
                for (b = -1; b <= 1; b++) {
                    if (a == 0 && b == 0) continue;
                    if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) { // nao sai dos limites
                        other = board[x + a][y + b];
                        if (other == 0)     // se vazio, adiciona
                            movesAtk.insert(movesAtk.end(), {x + a, y + b});
                        else if ((other > 0 ? -1 : 1) != (piece > 0 ? -1 : 1)) { // se inimigo, adiciona e sai
                            movesAtk.insert(movesAtk.end(), {x + a, y + b});
                            break;
                        } else // sai
                            break;
                    }
                }
            break;
    }

    return movesAtk;
}

bool underAttack(vector<Vector2i> pos_pieces){
    /* \brief Check if any of the positions is being attacked.
     *
     * \param pos_pieces: The set of postitions
     */
    int other;
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            other = board[x][y];
            if ((other > 0 ? 1 : 0) != turn){
                for (Vector2i pos : pos_pieces)
                    for (Vector2i pos_atk : getMovesAtk(x, y))
                        if (pos == pos_atk)
                            return true;
            }
        }
    }
    return false;
}
    
vector<Vector2i> getMovesNeutral(int x, int y) {
    vector<Vector2i> movesNeutral;
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
                        movesNeutral.insert(movesNeutral.end(), {x, y + b});
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
                    vector<Vector2i> moves =  {{2, y}, {3, y}, {4, y}};
                    if (!underAttack(moves))
                        movesNeutral.insert(movesNeutral.end(),{2, y});
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
                    vector<Vector2i> moves =  {{4, y}, {5, y}, {6, y}};
                    if (!underAttack(moves))
                        movesNeutral.insert(movesNeutral.end(),{6, y});
                }        
            }
            break;
    }
    return movesNeutral;
}

vector<Vector2i> getMoves(int x, int y) {
    /* Return attack and neutral (if existing) moves considering the chess' movement rules [1]_. 
	 *	
     * [1] this moves may not be allowed.
	 */
    vector<Vector2i> moves;
    vector<Vector2i> atk;
    vector<Vector2i> neutral;
    int piece = board[x][y];
    switch (abs(piece)%7) {
        case 1:
        case 6:     
            atk = getMovesAtk(x, y);
            neutral = getMovesNeutral(x, y);
            atk.insert( atk.end(), neutral.begin(), neutral.end() );
            break;
        default:
            atk = getMovesAtk(x, y);
            break;
    }
    moves = atk;
    return moves;
}


// Função updateBoard 
void updateBoard(int board[8][8]) {  
    Texture texture;
    Sprite sprite;
    for (int x=0; x < 8; x++)
        for (int y=0; y < 8; y++){
            int p = board[y][x];
            if (p == 0) continue;
            
            if (!texture.loadFromFile("Images/" + pieces_names[abs(p)%7 - 1] + (p > 0 ? 'W' : 'B') + ".png")){
                cout << "0x1c0e6ae ERROR: Please reboot your pc and unnistall your operating system!" << endl;
            }
            sprite.setTexture(texture);

            sprite.setScale(    
                (float)windowWidth/sprite.getLocalBounds().width/8,
                (float)windowHeight/sprite.getLocalBounds().height/8);

            sprite.setPosition(
                (float)sprite.getGlobalBounds().width*y, 
                (float)sprite.getGlobalBounds().height*x);
            window.draw(sprite);
        }
}

int main() {
    // Creates Window
    // Creates Object of type Event
    Event event;
    bool running = true;
    Texture texture;
    if (!texture.loadFromFile("Images/board-brown.png")){
        cout << "0x1c0e6ae ERROR: Please reboot your pc and unnistall your operating system!" << endl;
    }
    Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(
            (float)windowWidth/sprite.getLocalBounds().width,
            (float)windowHeight/sprite.getLocalBounds().height);


    //// ZONA DE TESTOSTERONA ////////////////
    vector<Vector2i> testosterona = getMoves(7, 6);  // peao branco
    cout << "Zona de Testosterona "<< endl;
    for(int i =0 ; i<testosterona.size();i++){
        cout << testosterona.at(i).x << "  " << testosterona.at(i).y << endl;
    }
    cout << endl;
    
    // FIM DE ZONA ESCROTO /////////
    Vector2f oldPos;
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
                window.draw(sprite);
                updateBoard(board);
                window.display();
                break;
            case Event::MouseButtonPressed:
                if(event.mouseButton.button==Mouse::Left){
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    window.draw(sprite);
                    updateBoard(board);
                    window.display();
                }
                break;
            case Event::MouseButtonReleased:

                if(event.mouseButton.button==Mouse::Left){
                    break;
                }
            // we don't process other types of events
            default:
                break;
        }
        // Loading texture
        window.setFramerateLimit(60);
    }
    return 0;
}
