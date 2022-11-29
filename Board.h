/*
This program uses the graphics library, SFML, to visualize the Breadth First Search (BFS) shortest path algorithm
for an undirected and unweighted graph.  The program displays a grid of tiles and allows the user to select a source, 
or starting tile, and a destination, or ending tile.  The user can also select a set of "obstacle" tiles which cannot
be reached or traversed.  The user can also select one of two graph implementations: a graph implemented as a map, 
or a graph implemented as a linked list (LL).  After finding the shortest path, the program will highlight the shortest
path found and display the time taken for the algorithm to finish.  This way, the user can compare which graph
implementation allowed for a faster path retrieval.  The user can also reset the board to "play again".
Note that "tiles" and "vertices" will be used interchangeably.
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <chrono>

using std::map;
using std::set;
using std::pair;
using std::string;
using std::vector;
using namespace std::chrono;

const float borderThickness = 0.5; // Thickness of tile's border.
const int tilesX = 50; // Number of tiles in each row.  Normally 250, but 25 to make the tiles easier to see.
const int tilesY = 50; // Number of rows of tiles.  Normally 250.
const float xSize = (1710.f / tilesX) - 1; // Length of each tile.
const float ySize = (1080.f / tilesX) - 1; // Height of each tile.

class Board
{
    struct Tile
    {
        sf::RectangleShape tile; // The SFML rectangle object.
        sf::Vector2f tilePosition;
        bool isSource; // True if tile is the source (i.e. starting tile), false otherwise.
        bool isDest; // True if tile is the destination (i.e. end tile), false otherwise.
        bool isObst; // True if the tile is an obstacle, false otherwise.
        int index; // Unique for each vertex. Goes from 0 to (number of vertices - 1), left to right for each row.
        Tile();
        void setTileColor(sf::Color c); // Sets fill color.  Black for unselected tile, Magenta for obstacle, Green for source, Red for destination. 
    };

    struct Node // Linked list implementation of the graph will consist of these Nodes.  Each node points to its nearest neighbor nodes.
    {
        Tile* tPtr; // Each node has a tile pointer as a value.
        Node* right = nullptr; // Node directly to the right.
        Node* left = nullptr; // Node directly to the left.
        Node* up = nullptr; // Node directly above.
        Node* down = nullptr; // Node directly below.
        Node* topLeft = nullptr; // Node up and to the left.
        Node* topRight = nullptr; // Node up and to the right.
        Node* botLeft = nullptr; // Node below and to the left.
        Node* botRight = nullptr; // Node below and to the right.
        Node(Tile* t) : tPtr(t) {}
    };

    private:
        map<Tile*, set<Tile*>> graphMap; // Map implementation of graph.  Maps from Tile pointer to set of Tile pointers which are the nearest neighbors.
        Node* head; // Head node of linked list (LL) graph implementation.  It will always point to the tile at index 0 (row 0, column 0).
        map<pair<int, int>, pair<Tile*, Node*>> posToTile; // Map from {i, j} grid position to its associated Tile and Node. 
        set<Tile*> obstacles; // Set of obstacle tiles.
        vector<Tile*> shortestPath; // Vector of tiles where each tile is part of the shortest path.
        sf::Text text1; // Text prompting user to select source/destination.
        sf::Text text2; // Text prompting user to select obstacles.
        sf::Text text3; // Text prompting user to select graph implementation type.
        sf::Text text4; // Text prompting user to reset board.
        sf::Text text5; // Text prompting user to to find shortest path.
        sf::Text text6; // Text confirming graph implementation selection (linked list or map).
        sf::Text text7; // Text describing result of shortest path.
        sf::Text text8; // Text describing result of shortest path.
        sf::Text text9; // Text describing result of time taken.
        sf::Text text10; // Text describing result of time taken.
        sf::Text text11; // Text prompting user to try again.
        sf::Sprite goSprite; // Sprite representing the "Go" button to start shortest path.
        sf::Texture goTexture; // Go button associated texture.
        sf::Sprite resetSprite; // Sprite representing the "Reset" button to reset the board.
        sf::Texture resetTexture; // Reset button associated texture.
        sf::Sprite linkedListSprite; // Sprite representing linked list button.
        sf::Texture linkedListTexture; // Linked list button associated texture.
        sf::Sprite mapSprite; // Sprite representing map button.
        sf::Texture mapTexture; // Map button associated texture.
        sf::Sprite tryAgainSprite; // Sprite representing Try Again button.
        sf::Texture tryAgainTexture; // Try Again button associated texture.
        duration<double, std::milli> duration; // Time taken by algorithm.
        bool mapSelected; // True if map implementation was selected, false otherwise.
        bool linkedListSelected; // True if linked list implementation was selected, false otherwise.
        bool goButtonClicked; // True when Go button is clicked.  Becomes false when program starts and when board is reset.
        bool tryAgainClicked; // True when Try Again button is clicked.  Is false when program starts and after reset is selected.
        Tile* source; // Source tile, as selected by user.
        Tile* destin; // Destination tile, as selected by user.
        void makeGraphs(); // Constructs both graph implementations.  Runs in the Board constructor.
        void insertEdges(int i, int j); // Inserts edges from tile at position {i, j} to its (up to) 8 nearest neighbors.
        void setLLPointers(int i, int j); // Sets the (up to) 8 pointers of each linked list node to its nearest neighbors.
        void displayBoard(sf::RenderWindow& window); // Displays the current state of the board to the user.
        void displayText(sf::RenderWindow& window); // Displays the text to the user.
        int* shortestPathGraph(); // Finds the shortest path for the map implementation.
        Node* traverseLL(int index); // Traverses linked list from head node to node at index.
        int* shortestPathLL(); // Finds the shortest path for the linked list implementation.
        int* shortestPathNodes(Node* start, Node* end); // Main function for finding the shortest path for the linked list implementation.
        void displayShortestPath(int* p); // Uses the shortest path array, p, and displays the shortest path tiles.
        void resetBoard(); // Resets board with all selections to default.

    public:
        Board(); // Constructor.
        ~Board(); // Destructor.
        void play(sf::RenderWindow& window); // Runs in main.cpp and is responsible for all actions (displays board, accepts input, displays results).
};