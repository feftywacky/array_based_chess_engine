#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include "engine.hpp"
#include "ai.hpp"

using namespace std;
using namespace sf;

int BOARD_WIDTH = 1024;
int BOARD_HEIGHT = 1024;
int DIMENSION = 8; // 8x8 chess board
int SQ_SIZE = BOARD_WIDTH/DIMENSION;
int MAX_FPS = 75;
std::map<std::string, sf::Texture> IMAGES;


void loadImages()
{

    
    array<string, 12> pieces = {"bR", "bN", "bB", "bQ", "bK", "bp", "wR", "wN", "wB", "wQ", "wK", "wp"};

    for (int i = 0; i < pieces.size(); i++)
    {
        static sf::Texture texture; // static for textures to persist outside the function
        if (!texture.loadFromFile("chess_pieces/" + pieces[i] + ".png"))
        {
            std::cout << "Failed to load chess pieces" << std::endl;
            return;
        }

        IMAGES[pieces[i]] = texture; // Store the sprite in the IMAGES map

    }
}

void drawPieces(sf::RenderWindow& gameWindow, const std::array<std::array<std::string, 8>, 8>& board)
{
    for (int r=0;r<DIMENSION;r++)
    {
        for (int c=0;c<DIMENSION;c++)
        {
            std::string piece = board[r][c];
            if (piece != "--")
            {
                // create sprite, set texture, and set posiition; 
                sf::Sprite sprite;
                
                // sprite.setTextureRect(sf::IntRect (c*SQ_SIZE,r*SQ_SIZE,SQ_SIZE,SQ_SIZE));
                sprite.setTexture(IMAGES[piece]);
                sprite.setPosition(c*SQ_SIZE,r*SQ_SIZE);

                // set texture scale
                // get original size of texture;
                sf::Vector2u textureSize = IMAGES[piece].getSize();
                sf::Vector2f scaleFactor(SQ_SIZE/textureSize.x, SQ_SIZE/textureSize.y);
                
                sprite.setScale(scaleFactor);

                gameWindow.draw(sprite);
            }
        }
    }
}


void drawBoard(sf::RenderWindow& gameWindow)
{
    sf::Color colours[2];
    colours[0] = sf::Color::White;
    colours[1] = sf::Color(180,180,180); // grey
    
    for (int r=0;r<DIMENSION;r++)
    {
        for (int c=0;c<DIMENSION;c++)
        {
            sf::Color colour = colours[(r+c)%2];

            sf::RectangleShape sq(sf::Vector2f(SQ_SIZE,SQ_SIZE));
            sq.setPosition(r * SQ_SIZE, c * SQ_SIZE);
            sq.setFillColor(colour);

            gameWindow.draw(sq);
        }
    }
}

void highlightSquares(sf::RenderWindow& gameWindow, const GameState& gameState,
                      std::vector<MovePiece> validMoves, std::vector<int> sq_selected, 
                      bool inCheck, std::vector<MovePiece> moveLog)
{
    // highlight from where to where that the piece moved to
    if (moveLog.size() != 0 )
    {
        MovePiece lastMove = moveLog.back();
        sf::RectangleShape surfaceLastMove(sf::Vector2f(SQ_SIZE,SQ_SIZE));
        surfaceLastMove.setFillColor(sf::Color(0, 255, 0, 75));
        surfaceLastMove.setPosition(lastMove.startCol * SQ_SIZE, lastMove.startRow * SQ_SIZE);
        gameWindow.draw(surfaceLastMove);

        surfaceLastMove.setPosition(lastMove.endCol * SQ_SIZE, lastMove.endRow * SQ_SIZE);
        gameWindow.draw(surfaceLastMove);
    }


    if (!sq_selected.empty())
    {
        int r = sq_selected[0];
        int c = sq_selected[1];
        std::string colour_to_move;

        // highlighting piece 
        if (gameState.whiteToMove)
            colour_to_move = "w";
        else
            colour_to_move = "b";
        
        

        if (gameState.board[r][c].substr(0,1) == colour_to_move)
        {
            sf::RectangleShape surface(sf::Vector2f(SQ_SIZE,SQ_SIZE));
            surface.setFillColor(sf::Color(4, 217, 255, 100));
            surface.setPosition(c * SQ_SIZE, r * SQ_SIZE);
            gameWindow.draw(surface);

            // highlight possible moves
            surface.setFillColor(sf::Color(255,255,59,100));
            for (int i=0;i<validMoves.size();i++)
            {
                MovePiece move = validMoves[i];
                if (move.startRow == r && move.startCol == c)
                {
                    surface.setPosition(move.endCol * SQ_SIZE, move.endRow * SQ_SIZE);
                    gameWindow.draw(surface);
                }
            }
        }

    
    }

    if (gameState.inCheck)
        {   
            int r;
            int c;
            if (gameState.whiteToMove)
            {
                r = gameState.wKingLocation[0];
                c = gameState.wKingLocation[1];
            }
            else
            {
                r = gameState.bKingLocation[0];
                c = gameState.bKingLocation[1];
            }

            sf::RectangleShape surfaceInCheck(sf::Vector2f(SQ_SIZE, SQ_SIZE));
            surfaceInCheck.setFillColor(sf::Color(255, 0, 0, 130));
            surfaceInCheck.setPosition(c * SQ_SIZE, r * SQ_SIZE);
            gameWindow.draw(surfaceInCheck);
            
        }
}


void drawGameState(sf::RenderWindow& gameWindow, const GameState& gameState, std::vector<MovePiece> validMoves, std::vector<int> sq_selected)
{
    drawBoard(gameWindow);
    highlightSquares(gameWindow, gameState, validMoves, sq_selected, gameState.inCheck, gameState.moveLog);
    drawPieces(gameWindow, gameState.board);

}



int main()
{
    loadImages();
    std::vector<int> sq_selected; // last user click, (row, col)
    std::vector<std::vector<int>> player_clicks; // 2 tuples [ (,) , (,)] from square, to square
    bool moveMade = false;
    bool moveUndone = false;

    bool gameOver = false;
    bool  isHumanTurn;
    GameState gameState;

    std::vector<MovePiece> validMoves = gameState.getValidMoves();

    sf::RenderWindow window(sf::VideoMode(BOARD_WIDTH, BOARD_HEIGHT), "CHESS", sf::Style::Close | sf::Style::Titlebar);
    
    // AI 

    bool humanPlayerOne = true;
    bool humanPlayerTwo = false;

    bool isAI_thinking = false;
    
    

    while (window.isOpen()) // main game loop
    {
        sf::Event event;
        isHumanTurn = (gameState.whiteToMove && humanPlayerOne) || (!gameState.whiteToMove && humanPlayerTwo);

        while (window.pollEvent(event)) // handle events
        {
            
            if (event.type == sf::Event::Closed)
                window.close(); 


            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) // mouse click
            {
                if (!gameOver)
                {
                    sf::Vector2i mouse_location = sf::Mouse::getPosition(window);
                    int mouse_row = mouse_location.y / SQ_SIZE;
                    int mouse_col = mouse_location.x / SQ_SIZE;
                    
                    if (!player_clicks.empty() && player_clicks[0][0] == mouse_row && player_clicks[0][1] == mouse_col || mouse_col > 8)
                    {   
                        // deselect the square
                        sq_selected.clear();
                        player_clicks.clear();
                    }

                    else // select square
                    {
                        sq_selected.clear();
                        sq_selected.push_back(mouse_row);
                        sq_selected.push_back(mouse_col);
                        player_clicks.push_back(sq_selected);
                    }
                    
                    if (player_clicks.size() == 2 && isHumanTurn)
                    {
                        MovePiece move = MovePiece(player_clicks[0], player_clicks[1], gameState.board);
                        for (int i=0;i<validMoves.size();i++)
                        {
                            if (move.equals(validMoves[i])) // need to override equals method
                            {
                                gameState.makeMove(validMoves[i]);
                                moveMade = true;
                                sq_selected.clear();
                                player_clicks.clear();
                            }
                        }

                        
                        if (!moveMade)
                        {
                            sq_selected = player_clicks[1];
                            player_clicks.clear();
                            player_clicks.push_back(sq_selected);
                            
                        }
                        
                    }
                } // game over flag
            } // end of mouse button left click

            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) // left arrow key
            {
                gameState.undoMove();
                sq_selected.clear();
                player_clicks.clear();
                moveMade = true;
                gameOver = false;
                moveUndone = true;
            } // end of left arrow key
            
            
            
        }// END OF EVENT HANDLING
        

        // ENGINE MOVE
        if (!gameOver && !isHumanTurn && !moveUndone)
        {
            if (!isAI_thinking)
            {
                isAI_thinking = true;
                std::cout<<"THINKING..."<<std::endl;
                MovePiece ai_move = findBestMove(gameState);
                gameState.makeMove(ai_move);
                moveMade = true;
                isAI_thinking = false;
            }   
        }




        if (moveMade)
        {

            // const int numIterations = 100; // Number of times to run the function

            // auto startTotal = std::chrono::high_resolution_clock::now(); // Record start time for total

            // for (int i = 0; i < numIterations; ++i) {
            //     gameState.getValidMoves(); // Call your function
            // }

            // auto endTotal = std::chrono::high_resolution_clock::now(); // Record end time for total

            // // Calculate the total duration
            // std::chrono::duration<double> totalDuration = endTotal - startTotal;

            // std::cout << "Total elapsed time for " << numIterations << " runs: " << totalDuration.count() << " seconds" << std::endl;

            validMoves = gameState.getValidMoves();
            moveMade = false;
        }

        window.clear();
        drawGameState(window, gameState, validMoves, sq_selected);

        if (gameState.checkMate || gameState.staleMate)
        {
            gameOver = true;
        }

        window.display();
    }
}


// #include <iostream>
// #include <chrono>
// #include <string>

// using std::cout, std::cin, std::endl;

// std::string talk()
// {
//     std::string str1 = "hello";
//     return str1;
// }

// int main() {
//    cout << talk() << endl; 
//    clock_t time;
//    time = clock();

// //    std::string board[8][8] = {
// //            "bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR",
// //            "bp", "bp", "bp", "bp", "bp", "bp", "bp", "bp",
// //            "--", "--", "--", "--", "--", "--", "--", "--",
// //            "--", "--", "--", "--", "--", "--", "--", "--",
// //            "--", "--", "--", "--", "--", "--", "--", "--",
// //            "--", "--", "--", "--", "--", "--", "--", "--",
// //            "wp", "wp", "wp", "wp", "wp", "wp", "wp", "wp",
// //            "wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"
// //    };

// //    cout << sizeof(board[0])/sizeof(std::string) << endl;
// //    for (int r=0;r<8;r++)
// //    {
// //        for (int c=0;c<8;c++)
// //        {
// //            if (board[r][c] != "--")
// //            {
// //                cout << board[r][c]<< "\n";
// //            }
// //        }
// //    }



//    cout << (double)time/CLOCKS_PER_SEC << "SECONDS";
//    return 0;
// }