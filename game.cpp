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
int board[8][8] = {
        {-11, -2, -3,-13, -5, -3, -2,-11},
        { -1, -1, -1, -1, -1, -1, -1, -1},
        {  0,  0,  0,  0,  0,  0,  0,  0},
        {  0,  0,  0,  0,  0,  0,  0,  0},
        {  0,  0,  0,  0,  0,  0,  0,  0},
        {  0,  0,  0,  0,  0,  0,  0,  0},
        {  1,  1,  1,  1,  1,  1,  1,  1}, 
        { 11,  2,  3, 13,  5,  3,  2, 11}
    };

// pawn = 1 / 8
// knight = 2 
// bishop = 3
// rook = 4 / 11
// queen = 5
// king = 6 / 13

vector<Vector2i> getMovesAtk(int x, int y) {
    int a, b, k, other;
    int piece = board[x][y];
    vector<Vector2i> movesAtk;

    switch (abs(piece)%7) {
        case 1:
            // diagonal capture
            a = piece > 0 ? -1 : 1;
            for (b = -1; b <= 1; b += 2)
                if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8) { // nao sai dos limites
                    other = board[x + a][y + b];
                    if (other == 0) { // está vazio
                        other = board[x][y + b];
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

vector<Vector2i> getMovesNeutral(int x, int y) {
    vector<Vector2i> movesNeutral;
    int a, b, other;
    int piece = board[x][y];
    switch (abs(piece)%7) {
        case 1:
            // one-two-squares advance movement
            for (a = 1; a <= 2; a++) {
                a *= piece > 0 ? -1 : 1;
                if (0 <= x + a && x + a < 8){
                    other = board[x + a][y];
                    if (other == 0) {
                        movesNeutral.insert(movesNeutral.end(), {x + a, y});
                        if(x != 1 + 5*(piece > 0 ? 1 : 0)) break;
                    }
                } else break;
            }
            break;
        case 6:
            break;
    }
    return movesNeutral;
}

vector<Vector2i> getMoves(int x, int y) {
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
                (float)sprite.getGlobalBounds().width*x, 
                (float)sprite.getGlobalBounds().height*y);
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
    vector<Vector2i> testosterona = getMoves(6, 7);  // pica cavalo branco
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

/*
    def get_moves_atk(self, x, y):
		""" Return attack moves. """
		piece = self.board[x][y]
		moves_atk = set()

		if piece[NAME] == 'P':
			# diagonal capture
			b = (-1)**int(piece[COLOR])
			for a in (-1, 1):
				if 0 <= x + a < 8 and 0 <= y + b < 8:
					other = self.board[x + a][y + b]
					if other is not None and other[COLOR] != piece[COLOR]:
						moves_atk.add((x + a, y + b))
					elif other is None:
						other = self.board[x + a][y + b - (-1)**int(piece[COLOR])]
						if other is not None and other[NAME] == 'P' and other[EN_PASSANT] == '1':
							moves_atk.add((x + a, y + b))
		elif piece[NAME] == 'N':
			# L-shape movement
			for a in (-2, -1, 1, 2):
				for b in (-2, -1, 1, 2):
					if abs(a) == abs(b): continue
					if 0 <= x + a < 8 and 0 <= y + b < 8:
						other = self.board[x + a][y + b]
						if other is None or other[COLOR] != piece[COLOR]:
							moves_atk.add((x + a, y + b))
		elif piece[NAME] == 'B':
			# diagonal movement
			for a in (-1, 1):
				for b in (-1, 1):			
					k = 1
					while 0 <= x + a*k < 8 and 0 <= y + b*k < 8:
						other = self.board[x + a*k][y + b*k]
						if other is None:
							moves_atk.add((x + a*k, y + b*k))
						elif other[COLOR] != piece[COLOR]:
							moves_atk.add((x + a*k, y + b*k))
							break
						else:
							break
						k += 1
		elif piece[NAME] == 'R':
			# cross movement
			for a in (-1, 0, 1):
				for b in (-1, 0, 1):
					if abs(a) == abs(b): continue
					k = 1
					while 0 <= x + a*k < 8 and 0 <= y + b*k < 8:
						other = self.board[x + a*k][y + b*k]
						if other is None:
							moves_atk.add((x + a*k, y + b*k))
						elif other[COLOR] != piece[COLOR]:
							moves_atk.add((x + a*k, y + b*k))
							break
						else:
							break
						k += 1
		elif piece[NAME] == 'Q':
			# cross-diagonal movement
			for a in (-1, 0, 1):
				for b in (-1, 0, 1):
					if a == b == 0: continue
					k = 1
					while 0 <= x + a*k < 8 and 0 <= y + b*k < 8:
						other = self.board[x + a*k][y + b*k]
						if other is None:
							moves_atk.add((x + a*k, y + b*k))
						elif other[COLOR] != piece[COLOR]:
							moves_atk.add((x + a*k, y + b*k))
							break
						else:
							break
						k += 1
		else:
			# one-square cross-diagonal movement
			for a in (-1, 0, 1):
				for b in (-1, 0, 1):
					if a == b == 0: continue
					if 0 <= x + a < 8 and 0 <= y + b < 8:
						other = self.board[x + a][y + b]
						if other is None or other[COLOR] != piece[COLOR]:
							moves_atk.add((x + a, y + b))

		return moves_atk

	def get_moves_neutral(self, x, y):
		""" Return neutral moves. """
		piece = self.board[x][y]
		moves_neutral = set()

		if piece[NAME] == 'P':
			# one-two-squares advance movement
			for b in (1, 2):
				b *= (-1)**int(piece[COLOR])
				if 0 <= y + b < 8:
					other = self.board[x][y + b]
					if other is None:
						moves_neutral.add((x, y + b))
						if y != 1 + 5*int(piece[COLOR]): 
							break
					else:
						break
		else:
			if piece[HAS_MOVED] == '1':
				return moves_neutral

			# castle
			other = self.board[0][y]
			if other is not None and other[NAME] == 'R' and other[HAS_MOVED] == '0':
				# no piece between this rook and king
				can_move = True
				for i in range(3):
					if self.board[1 + i][y] is not None:
						can_move = False
						break
		
				if can_move:
					# enemy attack range doesn't reach squares between this rook and king
					if not self.under_attack({(n, y) for n in range(1, x + 1)}):
						moves_neutral.add((2, y))

			other = self.board[7][y]
			if other is not None and other[NAME] == 'R' and other[HAS_MOVED] == '0':
				# no piece between this rook and king
				can_move = True
				for i in range(2):
					if self.board[6 - i][y] is not None:
						can_move = False
						break

				if can_move:
					# enemy attack range doesn't reach squares between this rook and king
					if not self.under_attack({(n, y) for n in range(x, 7)}):
						moves_neutral.add((6, y))

		return moves_neutral	

	def get_moves(self, x, y):
		""" Return attack and neutral (if existing) moves considering the chess' movement rules [1]_. 
		
		[1] this moves may not be allowed.
		"""
		piece = self.board[x][y]

		if piece[NAME] in ('P', 'K'):
			return self.get_moves_atk(x, y) | self.get_moves_neutral(x, y)
		return self.get_moves_atk(x, y)	
*/