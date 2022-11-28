
#include "Board.h"

int main()
{
    // Note: When program runs, a window will open.  Do not resize the window.  Please keep as-is.
    Board board; // Constructs a board having default values.
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "FIND SHORTEST PATH VIA BFS!"); // Create a window object.
    board.play(window); // Plays the "game", accepting user input, displaying results, etc.
    return 0;
}