#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <vector>
#include <string>
#include "tables.cpp"

using namespace sf;
using namespace std;

// prototype functions
vector<bool> makeMove(int x, int y, int n, int m);
vector<pair<int, int>> getMovesLegal(int x, int y);
map<pair<int, int>, vector<pair<int, int>>> getAllMovesLegal();
bool underAttack(vector<pair<int, int>> pos_pieces);
bool isCheck();
int isCheckmate();
vector<pair<int, int>> getMovesAtk(int x, int y);
vector<pair<int, int>> getMovesNeutral(int x, int y);
vector<pair<int, int>> getMoves(int x, int y);
void promote(int x, int y);
int evaluate();

// final variables
const int WIN_WIDTH = 600;
const int WIN_HEIGHT = 600;

// classes
class TextureManager
{
private:
    map<string, Texture> textures;

public:
    bool add(string name, string path, bool smooth)
    {
        Texture t;
        if (!t.loadFromFile(path))
        {
            cout << "0x1C0E6AE ERROR: Please reboot your pc and unnistall your operating system!" << endl;
            return false;
        }
        t.setSmooth(smooth);
        textures[name] = t;
        return true;
    }

    bool contains(string name)
    {
        if (textures.find(name) != textures.end())
            return true;
        return false;
    }

    Texture &get(string name)
    {
        return textures.at(name);
    }
};

class SpriteManager
{
private:
    map<pair<int, int>, Sprite> sprites;

public:
    void add(pair<int, int> key, Sprite sprite)
    {
        sprite.setPosition(
            WIN_WIDTH / 8.0 * key.first,
            WIN_HEIGHT / 8.0 * key.second);
        sprites[key] = sprite;
    }

    void add(pair<int, int> key, Texture &texture)
    {
        Sprite sprite;
        sprite.setTexture(texture);
        sprite.setScale(
            WIN_WIDTH / 8.0 / sprite.getLocalBounds().width,
            WIN_HEIGHT / 8.0 / sprite.getLocalBounds().height);

        add(key, sprite);
    }

    void remove(pair<int, int> key)
    {
        sprites.erase(key);
    }

    bool contains(pair<int, int> key)
    {
        if (sprites.find(key) != sprites.end())
            return true;
        return false;
    }

    Sprite &get(pair<int, int> key)
    {
        return sprites.at(key);
    }

    map<pair<int, int>, Sprite>::iterator begin()
    {
        return sprites.begin();
    }

    map<pair<int, int>, Sprite>::iterator end()
    {
        return sprites.end();
    }
};

// global variables
bool turn = true;
int board[8][8] = {
    {-11, -1, 0, 0, 0, 0, 1, 11}, //   pawn = 1/8     isn't/is en passant
    {-2, -1, 0, 0, 0, 0, 1, 2},   // knight = 2
    {-3, -1, 0, 0, 0, 0, 1, 3},   // bishop = 3
    {-5, -1, 0, 0, 0, 0, 1, 5},   //   rook = 4/11    has/hasn't moved
    {-13, -1, 0, 0, 0, 0, 1, 13}, //  queen = 5
    {-3, -1, 0, 0, 0, 0, 1, 3},   //   king = 6/13    has/hasn't moved
    {-2, -1, 0, 0, 0, 0, 1, 2},
    {-11, -1, 0, 0, 0, 0, 1, 11}};
string piecesNames[6] = {"pawn", "knight", "bishop", "rook", "queen", "king"};
Sprite boardSprite;
TextureManager textureManager;
SpriteManager spriteManager;
RenderWindow window(VideoMode(WIN_WIDTH, WIN_HEIGHT), "CppChess");

// functions
void promote(int x, int y)
{
    cout << endl
         << "Promote To:" << endl
         << "(Q) Queen:" << endl
         << "(R) Rook:" << endl
         << "(B) Bishop:" << endl
         << "(K) Knight:" << endl;

    char res[1];
    string color = board[x][y] > 0 ? "W" : "B";

    while (1)
    {
        cout << ">>> ";
        scanf("%1s", res);
        for (char &c : res)
            c = toupper(c);

        if (res[0] == 'Q')
        {
            board[x][y] *= 5;
            spriteManager.get({x, y}).setTexture(
                textureManager.get("queen" + color));
            break;
        }
        else if (res[0] == 'R')
        {
            board[x][y] *= 4;
            spriteManager.get({x, y}).setTexture(
                textureManager.get("rook" + color));
            break;
        }
        else if (res[0] == 'B')
        {
            board[x][y] *= 3;
            spriteManager.get({x, y}).setTexture(
                textureManager.get("bishop" + color));
            break;
        }
        else if (res[0] == 'K')
        {
            board[x][y] *= 2;
            spriteManager.get({x, y}).setTexture(
                textureManager.get("knight" + color));
            break;
        }
        else
        {
            // Clean the input
            while (getchar() != '\n')
                ;
        }
    }
}

vector<bool> makeMove(int x, int y, int n, int m)
{
    /**
     *  Fix pieces' position and attributes after a move. 
     */
    bool captured = false, promoted = false;
    int other;
    if (board[n][m] != 0)
        captured = true;

    // Make move
    int piece = board[x][y];
    board[x][y] = 0;
    board[n][m] = piece;

    switch (abs(piece) % 7)
    {
    case 1:
        // Set pawn's ``en_passant`` attribute to true
        if (m == 3 + turn)
            board[n][m] = 8 * (piece > 0 ? 1 : -1);
        // Remove enemy pawns that were captured en passant
        // Acknowledge capture
        other = board[n][piece > 0 ? m + 1 : m - 1];
        if (abs(other) == 8)
        {
            board[n][piece > 0 ? m + 1 : m - 1] = 0;
            captured = true;
        }
        if (m == 7 * (!turn))
            promoted = true;
        break;
    case 4:
        board[n][m] = 4 * (piece > 0 ? 1 : -1);
        break;
    case 6:
        if (abs(piece) == 13)
        {
            //swap rook with the king
            if (n == 2)
            {
                //int rook = board[0][7*turn];
                board[0][7 * turn] = 0;
                board[3][7 * turn] = 4 * (piece > 0 ? 1 : -1);
            }
            else if (n == 6)
            {
                //int rook = board[7][7*turn];
                board[7][7 * turn] = 0;
                board[5][7 * turn] = 4 * (piece > 0 ? 1 : -1);
            }
        }
        board[n][m] = 6 * (piece > 0 ? 1 : -1); // king has moved
        break;
    }
    // Rechange enemy pawn's attribute ``en_passant`` to false
    for (int i = 0; i < 8; i++)
    {
        other = board[i][3 + (!turn)];
        if (abs(other) % 7 == 1 && (other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1))
            board[i][3 + !turn] = piece > 0 ? -1 : 1;
    }
    return {captured, promoted};
}

vector<pair<int, int>> getMovesLegal(int x, int y)
{
    /** 
     * Return a list with allowed moves for one piece. 
     */
    int temp_board[8][8];
    vector<pair<int, int>> movesLegal;
    vector<pair<int, int>> moves = getMoves(x, y);

    // Check if ``moves`` are already legal
    int piece = board[x][y];
    board[x][y] = 0;
    if ((abs(piece) % 7) != 6 && !isCheck())
    {
        movesLegal = moves;
    }

    // Filter the moves that are legal
    else
    {
        board[x][y] = piece;

        // Verify moves that stop the check
        for (pair<int, int> m : moves)
        {
            // Copy board to temp_board
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    temp_board[i][j] = board[i][j];

            makeMove(x, y, m.first, m.second);

            if (!isCheck())
                movesLegal.insert(movesLegal.end(), m);

            // Copy temp_board to board
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    board[i][j] = temp_board[i][j];
        }
    }
    board[x][y] = piece;
    return movesLegal;
}

map<pair<int, int>, vector<pair<int, int>>> getAllMovesLegal()
{
    /** 
     * Return a dictionary with allowed moves for each piece. 
     */
    int other;
    map<pair<int, int>, vector<pair<int, int>>> all_movesLegal;

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
        {
            other = board[x][y];
            if (other == 0)
                continue;

            if ((other > 0 ? 1 : 0) == turn)
            {
                all_movesLegal[{x, y}] = getMovesLegal(x, y);
            }
        }
    return all_movesLegal;
}

bool underAttack(vector<pair<int, int>> pos_pieces)
{
    /** 
     * Check if any of the positions is being attacked.
     *
     * @param pos_pieces The set of positions.
     */
    int other;
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            other = board[x][y];
            if ((other > 0 ? 1 : 0) != turn)
            {
                for (pair<int, int> pos : pos_pieces)
                    for (pair<int, int> pos_atk : getMovesAtk(x, y))
                        if (pos == pos_atk)
                            return true;
            }
        }
    }
    return false;
}

bool isCheck()
{
    /**
     * Check if king is being attacked.
     */
    int other;

    // Find king currently playing
    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
        {
            other = board[x][y];
            if ((abs(other) % 7) == 6 && (other > 0 ? 1 : 0) == turn)
                if (underAttack({{x, y}}))
                    return true;
        }
    return false;
}

int isCheckmate()
{
    /**
     * Return state of the game (0: no checkmate, 1: checkmate, 2: stalemate).
     */
    int checkmate;
    bool hasMoves = false;
    if (isCheck())
    {
        checkmate = 1;
        for (const auto m : getAllMovesLegal())
        {
            if (m.second.size() != 0)
            {
                checkmate = 0;
                break;
            }
        }
    }
    else
    {
        checkmate = 0;
        for (const auto m : getAllMovesLegal())
        {

            if (m.second.size() != 0)
            {
                hasMoves = true;
                break;
            }
        }
        if (!hasMoves)
            checkmate = 2;
    }
    return checkmate;
}

vector<pair<int, int>> getMovesAtk(int x, int y)
{
    /** 
     * Return moves that are hostile. 
     */
    int other, piece = board[x][y];
    vector<pair<int, int>> moves_atk;

    switch (abs(piece) % 7)
    {
    case 1:
        // diagonal capture
        for (int a : {-1, 1})
        {
            int b = piece > 0 ? -1 : 1;
            if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8)
            { // inner board
                other = board[x + a][y + b];
                if (other == 0)
                {
                    other = board[x + a][y];
                    if (abs(other) == 8) // pawn en_passant
                        moves_atk.insert(moves_atk.end(), {x + a, y + b});
                }
                else if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1)) // different teams
                    moves_atk.insert(moves_atk.end(), {x + a, y + b});
            }
        }
        break;
    case 2:
        // L-shape movement
        for (int a : {-2, -1, 1, 2})
            for (int b : {-2, -1, 1, 2})
            {
                if (abs(a) == abs(b))
                    continue;
                if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8)
                {
                    other = board[x + a][y + b];
                    if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1) || other == 0)
                    {
                        moves_atk.insert(moves_atk.end(), {x + a, y + b});
                    }
                }
            }
        break;
    case 3:
        // diagonal movement
        for (int a : {-1, 1})
            for (int b : {-1, 1})
            {
                for (int k = 1; 0 <= x + a * k && x + a * k < 8 && 0 <= y + b * k && y + b * k < 8; k++)
                {
                    other = board[x + a * k][y + b * k];
                    if (other == 0) // if empty, add
                        moves_atk.insert(moves_atk.end(), {x + a * k, y + b * k});
                    else if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1))
                    { // if enemy, add and break
                        moves_atk.insert(moves_atk.end(), {x + a * k, y + b * k});
                        break;
                    }
                    else
                        break;
                }
            }
        break;
    case 4:
        // cross movements
        for (int a : {-1, 0, 1})
            for (int b : {-1, 0, 1})
            {
                if (abs(a) == abs(b))
                    continue;
                for (int k = 1; 0 <= x + a * k && x + a * k < 8 && 0 <= y + b * k && y + b * k < 8; k++)
                {
                    other = board[x + a * k][y + b * k];
                    if (other == 0)
                        moves_atk.insert(moves_atk.end(), {x + a * k, y + b * k});
                    else if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1))
                    {
                        moves_atk.insert(moves_atk.end(), {x + a * k, y + b * k});
                        break;
                    }
                    else
                        break;
                }
            }
        break;
    case 5:
        // cross-diagonal movement
        for (int a : {-1, 0, 1})
            for (int b : {-1, 0, 1})
            {
                if (a == 0 && b == 0)
                    continue;
                for (int k = 1; 0 <= x + a * k && x + a * k < 8 && 0 <= y + b * k && y + b * k < 8; k++)
                {
                    other = board[x + a * k][y + b * k];
                    if (other == 0)
                        moves_atk.insert(moves_atk.end(), {x + a * k, y + b * k});
                    else if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1))
                    {
                        moves_atk.insert(moves_atk.end(), {x + a * k, y + b * k});
                        break;
                    }
                    else
                        break;
                }
            }
        break;
    case 6:
        // one-cross-diagonal movement
        for (int a : {-1, 0, 1})
            for (int b : {-1, 0, 1})
            {
                if (a == 0 && b == 0)
                    continue;
                if (0 <= x + a && x + a < 8 && 0 <= y + b && y + b < 8)
                {
                    other = board[x + a][y + b];
                    if (other == 0)
                        moves_atk.insert(moves_atk.end(), {x + a, y + b});
                    else if ((other > 0 ? 1 : -1) != (piece > 0 ? 1 : -1))
                        moves_atk.insert(moves_atk.end(), {x + a, y + b});
                }
            }
        break;
    }
    return moves_atk;
}

vector<pair<int, int>> getMovesNeutral(int x, int y)
{
    vector<pair<int, int>> moves_neutral;
    int a, b, other;
    int piece = board[x][y];
    switch (abs(piece) % 7)
    {
    case 1:
        // one-two-squares advance movement
        for (int c : {1, 2})
        {
            b = c * (piece > 0 ? -1 : 1);
            if (0 <= y + b && y + b < 8)
            {
                other = board[x][y + b];
                if (other == 0)
                {
                    moves_neutral.insert(moves_neutral.end(), {x, y + b});
                    if (y != 1 + 5 * (piece > 0 ? 1 : 0))
                        break; // só anda 1 pá frente
                }
                else
                    break;
            }
        }
        break;
    case 6:
        if (piece == 6)
            break;

        // castle
        other = board[0][y]; // queen side
        if (other == 11)
        {
            // no piece between this rook and king
            bool can_move = true;
            for (int i : {0, 1, 2})
                if (board[1 + i][y] != 0)
                {
                    can_move = false;
                    break;
                }
            if (can_move)
            {
                // enemy attack range doesn't reach squares between this rook and king
                vector<pair<int, int>> moves = {{2, y}, {3, y}, {4, y}};
                if (!underAttack(moves))
                    moves_neutral.insert(moves_neutral.end(), {2, y});
            }
        }
        other = board[7][y]; // king side
        if (other == 11)
        {
            // no piece between this rook and king
            bool can_move = true;
            for (int i : {0, 1})
                if (board[6 - i][y] != 0)
                {
                    can_move = false;
                    break;
                }
            if (can_move)
            {
                // enemy attack range doesn't reach squares between this rook and king
                vector<pair<int, int>> moves = {{4, y}, {5, y}, {6, y}};
                if (!underAttack(moves))
                    moves_neutral.insert(moves_neutral.end(), {6, y});
            }
        }
        break;
    }
    return moves_neutral;
}

vector<pair<int, int>> getMoves(int x, int y)
{
    /**
     *  Return attack and neutral (if existing) moves considering the chess' movement rules (this moves may not be allowed). 
	 */
    vector<pair<int, int>> moves;
    vector<pair<int, int>> neutral;
    int piece = board[x][y];
    switch (abs(piece) % 7)
    {
    case 1:
    case 6:
        moves = getMovesAtk(x, y);
        neutral = getMovesNeutral(x, y);
        moves.insert(moves.end(), neutral.begin(), neutral.end());
        break;
    default:
        moves = getMovesAtk(x, y);
        break;
    }
    return moves;
}

int evaluate()
{
    /** Return an evaluation[1]_[2]_ for the piece's positions in ``board``.
	 *	
	 *	https://www.chessprogramming.org/Evaluation
	 *	https://www.chessprogramming.org/Simplified_Evaluation_Function
     */
    int val = 0;
    int nPieces = 0;
    int centipawns;
    int other;
    bool brk;

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
        {
            if (board[x][y] == 0)
                continue;
            nPieces += 1;
            int piece = board[x][y];
            int i, j;

            if (piece > 0)
            {
                i = y;
                j = x;
            }
            else
            {
                i = 7 - y;
                j = 7 - x;
            }

            switch (abs(piece) % 7)
            {
            case 1:
                // Suppose right away it's an isolated, and then check if it actually is
                centipawns = 50 + EVAL_PAWN[i][j];

                // Check if doubled
                for (int m = 0; m < 8; m++)
                {
                    if (m == y)
                        continue;
                    other = board[x][m];
                    if (other != 0 && (other > 0 ? 1 : -1) == (piece > 0 ? 1 : -1) && abs(other) % 7 == 1)
                    {
                        centipawns -= 50;
                        break;
                    }
                }
                // Check if not isolated
                brk = false;
                for (int n = y - 1; n < y + 1; n++)
                {
                    if (!(0 <= n < 8))
                        continue;
                    for (int m = 0; m < 8; m++)
                    {
                        other = board[n][m];
                        if (other != 0 && (other > 0 ? 1 : -1) == (piece > 0 ? 1 : -1) && abs(other) % 7 == 1)
                        {
                            centipawns += 50;
                            brk = true;
                            break;
                        }
                    }
                    if (brk)
                        break;
                }
                // Check if blocked
                if (getMoves(x, y).size() == 0)
                    centipawns -= 50;
                break;
            case 2:
                centipawns = 320 + EVAL_KNIGHT[i][j];
                break;
            case 3:
                centipawns = 330 + EVAL_BISHOP[i][j];
                break;
            case 4:
                centipawns = 500 + EVAL_ROOK[i][j];
                break;
            case 5:
                centipawns = 900 + EVAL_QUEEN[i][j];
                break;
            case 6:
                if (nPieces > 8)
                    centipawns = EVAL_KING_MIDDLE[i][j];
                else
                    centipawns = EVAL_KING_END[i][j];
                break;
            }
            val += ((piece > 0 ? 1 : 0) == turn ? centipawns : -centipawns);
        }

    for (const auto m : getAllMovesLegal())
        val += 10 * m.second.size();
    turn = !turn;

    for (const auto m : getAllMovesLegal())
        val -= 10 * m.second.size();
    turn = !turn;

    return val;
}

void updateBoard(pair<int, int> curPos, pair<int, int> oldPos, pair<int, int> newPos, vector<pair<int, int>> movesLegal)
{
    Sprite sprite;

    // Draw board
    window.draw(boardSprite);

    // Draw marks
    sprite.setTexture(textureManager.get("active"));
    sprite.setScale(
        WIN_WIDTH / 8.0 / sprite.getLocalBounds().width,
        WIN_WIDTH / 8.0 / sprite.getLocalBounds().height);
    sprite.setPosition(
        WIN_WIDTH / 8.0 * curPos.first,
        WIN_HEIGHT / 8.0 * curPos.second);
    window.draw(sprite);
    sprite.setPosition(
        WIN_WIDTH / 8.0 * oldPos.first,
        WIN_HEIGHT / 8.0 * oldPos.second);
    window.draw(sprite);
    sprite.setPosition(
        WIN_WIDTH / 8.0 * newPos.first,
        WIN_HEIGHT / 8.0 * newPos.second);
    window.draw(sprite);

    // Draw moves
    for (int i = 0; i < movesLegal.size(); i++)
    {
        pair<int, int> move = movesLegal[i];
        if (board[move.first][move.second] != 0 || (abs(board[curPos.first][curPos.second]) == 1 && move.first != curPos.first))
            sprite.setTexture(textureManager.get("capture"));
        else
            sprite.setTexture(textureManager.get("circle"));
        sprite.setScale(
            WIN_WIDTH / 8.0 / sprite.getLocalBounds().width,
            WIN_WIDTH / 8.0 / sprite.getLocalBounds().height);
        sprite.setPosition(
            WIN_WIDTH / 8.0 * move.first,
            WIN_HEIGHT / 8.0 * move.second);
        window.draw(sprite);
    }

    // Draw pieces
    map<pair<int, int>, Sprite>::iterator it = spriteManager.begin();
    while (it != spriteManager.end())
    {
        window.draw(it->second);
        it++;
    }
}

/// start of DEBUG FUNCTIONS

void printBoard()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
            printf("%3d ", board[i][j]);
        cout << endl;
    }
    cout << endl;
}

void printAllMoves(map<pair<int, int>, vector<pair<int, int>>> turnMoves)
{
    for (const auto m : turnMoves)
    {
        cout << m.first.first << ',' << m.first.second;
        const char *name = piecesNames[abs(board[m.first.first][m.first.second]) % 7 - 1].c_str();
        printf("%7s : ", name);
        for (pair<int, int> p : m.second)
            cout << '(' << p.first << ',' << p.second << ") ";
        cout << endl;
    }
}

/// end of DEBUG FUNCTIONS

int main()
{
    textureManager.add("board", "Images/board-brown.png", false);
    textureManager.add("active", "Images/square-greenish.png", true);
    textureManager.add("capture", "Images/capture-green.png", true);
    textureManager.add("circle", "Images/circle-green.png", true);

    boardSprite.setTexture(textureManager.get("board"));
    boardSprite.setScale(
        (float)WIN_WIDTH / boardSprite.getLocalBounds().width,
        (float)WIN_HEIGHT / boardSprite.getLocalBounds().height);

    for (int x = 0; x < 8; x++)
        for (int y = 0; y < 8; y++)
        {
            int piece = board[x][y];
            if (piece == 0)
                continue;

            string name = piecesNames[abs(piece) % 7 - 1];
            string color = (piece > 0) ? "W" : "B";

            textureManager.add(name + color, "Images/" + name + color + ".png", true);
            spriteManager.add({x, y}, textureManager.get(name + color));
        }

    bool clicking = false;
    bool dragging = false;
    pair<int, int> pos;
    pair<int, int> curPos = {-1, -1};
    pair<int, int> oldPos = {-1, -1};
    pair<int, int> newPos = {-1, -1};
    map<pair<int, int>, vector<pair<int, int>>> turnMoves = getAllMovesLegal();
    Cursor cursor;
    Event event;

    printBoard();                           ///
    printAllMoves(turnMoves);               ///
    cout << "eval: " << evaluate() << endl; ///

    while (window.isOpen())
    {
        updateBoard(curPos, oldPos, newPos, turnMoves[curPos]);

        // gets the current event
        window.pollEvent(event);
        switch (event.type)
        {
        case Event::Closed:
            window.close();
            break;
        case Event::MouseButtonPressed:
            if (event.mouseButton.button == Mouse::Left)
            {
                clicking = true;
            }
            break;
        case Event::MouseButtonReleased:
            if (event.mouseButton.button == Mouse::Left)
            {
                pos = {(int)(8.0 * event.mouseButton.x / window.getSize().x), (int)(8.0 * event.mouseButton.y / window.getSize().y)};

                if (find(turnMoves[curPos].begin(), turnMoves[curPos].end(), pos) != turnMoves[curPos].end())
                {
                    if (clicking || dragging)
                    {
                        oldPos = curPos;
                        newPos = pos;

                        vector<bool> v = makeMove(oldPos.first, oldPos.second, newPos.first, newPos.second);
                        bool captured = v[0];
                        bool promoted = v[1];

                        turn = !turn;

                        if (int res = isCheckmate())
                        {
                            if (res == 1)
                                cout << (!turn ? "White" : "Black") << " Wins!" << endl;
                            else if (res == 2)
                                cout << "Draw!" << res << endl;
                        }

                        spriteManager.add(newPos, spriteManager.get(oldPos));
                        spriteManager.remove(oldPos);

                        if (captured)
                        {
                            // Remove pawn captured en passant
                            int piece = board[newPos.first][turn ? newPos.second - 1 : newPos.second + 1];
                            if (piece == 0)
                                spriteManager.remove({newPos.first, turn ? newPos.second - 1 : newPos.second + 1});
                        }
                        if (promoted)
                            promote(newPos.first, newPos.second);

                        turnMoves = getAllMovesLegal();

                        printBoard();                           ///
                        printAllMoves(turnMoves);               ///
                        cout << "eval: " << evaluate() << endl; ///

                        curPos = {-1, -1};
                        clicking = false;
                        dragging = false;
                    }
                }

                if (spriteManager.contains(pos))
                {
                    if (clicking)
                    {
                        curPos = pos;
                        clicking = false;
                    }
                    if (cursor.loadFromSystem(Cursor::Hand))
                        window.setMouseCursor(cursor);
                }
                else
                {
                    if (clicking)
                    {
                        curPos = {-1, -1};
                        clicking = false;
                    }
                    if (cursor.loadFromSystem(Cursor::Arrow))
                        window.setMouseCursor(cursor);
                }

                if (dragging)
                {
                    spriteManager.add(curPos, spriteManager.get(curPos));
                    dragging = false;
                }
            }
            break;
        case Event::MouseMoved:
            pos = {(int)(8.0 * event.mouseMove.x / window.getSize().x), (int)(8.0 * event.mouseMove.y / window.getSize().y)};

            if (spriteManager.contains(pos))
            {
                if (clicking)
                {
                    curPos = pos;
                    dragging = true;
                    clicking = false;
                }
                if (!dragging)
                    if (cursor.loadFromSystem(Cursor::Hand))
                        window.setMouseCursor(cursor);
            }
            else
            {
                if (clicking)
                    clicking = false;
                if (!dragging)
                    if (cursor.loadFromSystem(Cursor::Arrow))
                        window.setMouseCursor(cursor);
            }

            if (dragging)
            {
                spriteManager.get(curPos).setPosition(event.mouseMove.x - window.getSize().x / 16.0, event.mouseMove.y - window.getSize().x / 16.0);
                window.draw(spriteManager.get(curPos));
            }
            break;
        default:
            break;
        }

        window.display();
        window.setFramerateLimit(250);
    }
    return 0;
}

class Player
{
    void play();
};

/*


class AI : Player {
    private:
        int depth;

    AI(int depth) {
        this->depth = depth;
    }

    void play() {

    }
        


    int minmax() {

        


    }

};


class RealPlayer : Player{

    void play(){


    }

    
};



def play(self):
		self.timer.triggerStop()
		self.no_nodes = [0] * self.depth_final##
		self.quiescence = False

		game = gm.Game(self.game.turn, [list(row) for row in self.game.board], 
					   list(self.game.pos_kings))
		val, moves_best = self.negamax(game)#, depth=0, alpha=32767, beta=-32768)

		try:
			print(moves_best)
			x, y, n, m, _ = moves_best[-1]
		except ValueError:
			x, y, n, m, promotion, _ = moves_best[-1]
			
		self.board.origin = (x, y)
		self.board.destin = (n, m)
		
		captured, promoted = self.game.make_move(x, y, n, m)

		if captured:
			self.board.sound_capture.play()
		else:
			self.board.sound_move.play()
		if promoted:
			self.game.board[n][m] = promotion + str(self.game.turn)

		self.board.update()
		self.timer.triggerStop()
		return 



def negamax(self, game, depth=0, alpha=-32768, beta=32767):
		checkmate = game.checkmate()
		if checkmate == 1:
			self.no_nodes[depth - 1] += 1
			return game.evaluate() - 20000, []
		if checkmate == 2:
			self.no_nodes[depth - 1] += 1
			return 0, []
		# quiescence search
		if self.quiescence:
			self.no_nodes[depth - 1] += 1
			return game.evaluate(), []
		if depth == self.depth_final:
			self.no_nodes[depth - 1] += 1
			return game.evaluate(), []

		val_best, moves_best = -32768, []
		for x in range(8):
			for y in range(8):
				other = game.board[x][y]
				if other is None or other[gm.COLOR] != str(game.turn): continue

				moves_allowed = game.get_moves_allowed(x, y)
				
				for n, m in moves_allowed:
					self.quiescence = False
					game2 = gm.Game(game.turn, [list(row) for row in game.board], 
								    list(game.pos_kings))
					captured, promoted = game2.make_move(x, y, n, m)
					game2.turn = 0 if game2.turn else 1

					if not captured and depth in {1, 2}:
						self.quiescence = True


					if promoted:
						for promotion in ('N', 'B', 'R', 'Q'):
							game2.board[n][m] = promotion + str(self.game.turn)

							# recursiveness
							val, moves = self.negamax(game2, depth + 1, -beta, -alpha)
							val = -val

										
							if val_best < val:
								notation = other[gm.NAME]
								if captured:
									if notation == 'P':
										notation = chr(97 + x)
									notation += 'x'
								if notation == 'P':
									notation = ''

								val_best = val
								moves.append((x, y, n, m, promotion, notation))
								moves_best = moves
							alpha = max(alpha, val)
							
					else:
						# recursiveness
						val, moves = self.negamax(game2, depth + 1, -beta, -alpha)
						val = -val
						

						if val_best < val:
							notation = other[gm.NAME]
							if captured:
								if notation == 'P':
									notation = chr(97 + x)
								notation += 'x'
							if notation == 'P':
								notation = ''

							val_best = val
							moves.append((x, y, n, m, notation))
							moves_best = moves
						alpha = max(alpha, val)

					# alpha-beta prune
					if alpha >= beta:
						return val_best, moves_best

				game.board[x][y] = other

		return val_best, moves_best
*/